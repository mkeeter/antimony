#include <stdlib.h>
#include <stdio.h>

#include "triangulate.h"

#include "tree/tree.h"
#include "tree/eval.h"
#include "util/constants.h"

static const uint8_t VERTEX_LOOP[] = {6, 4, 5, 1, 3, 2, 6};

// Based on which vertices are filled, this map tells you which
// edges to interpolate between when forming zero, one, or two
// triangles.
// (filled vertex is first in the pair)
static const int EDGE_MAP[16][2][3][2] = {
    {{{-1,-1}, {-1,-1}, {-1,-1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // ----
    {{{ 0, 2}, { 0, 1}, { 0, 3}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // ---0
    {{{ 1, 0}, { 1, 2}, { 1, 3}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // --1-
    {{{ 1, 2}, { 1, 3}, { 0, 3}}, {{ 0, 3}, { 0, 2}, { 1, 2}}}, // --10
    {{{ 2, 0}, { 2, 3}, { 2, 1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // -2--
    {{{ 0, 3}, { 2, 3}, { 2, 1}}, {{ 2, 1}, { 0, 1}, { 0, 3}}}, // -2-0
    {{{ 1, 0}, { 2, 0}, { 2, 3}}, {{ 2, 3}, { 1, 3}, { 1, 0}}}, // -21-
    {{{ 2, 3}, { 1, 3}, { 0, 3}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // -210

    {{{ 3, 0}, { 3, 1}, { 3, 2}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 3---
    {{{ 3, 2}, { 0, 2}, { 0, 1}}, {{ 0, 1}, { 3, 1}, { 3, 2}}}, // 3--0
    {{{ 1, 2}, { 3, 2}, { 3, 0}}, {{ 3, 0}, { 1, 0}, { 1, 2}}}, // 3-1-
    {{{ 1, 2}, { 3, 2}, { 0, 2}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 3-10
    {{{ 3, 0}, { 3, 1}, { 2, 1}}, {{ 2, 1}, { 2, 0}, { 3, 0}}}, // 32--
    {{{ 3, 1}, { 2, 1}, { 0, 1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 32-0
    {{{ 3, 0}, { 1, 0}, { 2, 0}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 321-
    {{{-1,-1}, {-1,-1}, {-1,-1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 3210
};


// Performs trilinear interpolation, where d is an array of corner values
// on a unit cube (d[0] at (0,0,0), d[1] at (0,0,1), d[2] at (0,1,0), etc.
_STATIC_
float interpolate(const float d[8],
                  const float x, const float y, const float z)
{
    const float c00 = d[0]*(1-x) + d[4]*x;
    const float c01 = d[1]*(1-x) + d[5]*x;
    const float c10 = d[2]*(1-x) + d[6]*x;
    const float c11 = d[3]*(1-x) + d[7]*x;

    const float  c0 =  c00*(1-y) + c10*y;
    const float  c1 =  c01*(1-y) + c11*y;

    const float   c =   c0*(1-z) + c1*z;

    return c;
}


// Finds the zero crossing on the line between two vertices of a unit cube.
// d is sample values at the cube's corners.
_STATIC_
Vec3f zero_crossing(const float d[8],
                    const uint8_t v0, const uint8_t v1)
{
    const float x0 =  (v0 & 4) ? 1 : 0,
                y0 =  (v0 & 2) ? 1 : 0,
                z0 =  (v0 & 1) ? 1 : 0;

    const float dx = ((v1 & 4) ? 1 : 0) - x0,
                dy = ((v1 & 2) ? 1 : 0) - y0,
                dz = ((v1 & 1) ? 1 : 0) - z0;

    float p = 0.5;
    float step = 0.25;

    // Binary search along the edge to find the zero crossing
    for (int iteration=0; iteration < 16; ++iteration)
    {
        const float r = interpolate(d, x0 + p*dx, y0 + p*dy, z0 + p*dz);

        if      (r < -EPSILON)  p += step;
        else if (r > EPSILON)   p += step;
        else                    break;

        step /= 2;
    }

    return (Vec3f){x0 + p*dx, y0 + p*dy, z0 + p*dz};
}


_STATIC_
void triangulate_voxel(MathTree* tree, const Region r,
                       float** const verts, unsigned* const count,
                       unsigned* const allocated)
{
    // Recursive case: split into subregions and recurse
    if (r.voxels > 1)
    {
        Region octants[8];
        const uint8_t split = octsect(r, octants);
        for (int i=0; i < 8; ++i)
        {
            if (split & (1 << i))
            {
                triangulate_voxel(tree, octants[i],
                                  verts, count, allocated);
            }
        }
        return;
    }

    // Find corner distance values for this voxel
    float d[8];
    for (int i=0; i < 8; ++i)
    {
        d[i] = eval_f(tree, (i & 4) ? r.X[1] : r.X[0],
                            (i & 2) ? r.Y[1] : r.Y[0],
                            (i & 1) ? r.Z[1] : r.Z[0]);
    }

    // Loop over the six tetrahedra that make up a voxel cell
    for (int t=0; t < 6; ++t)
    {
        // Find vertex positions for this tetrahedron
        uint8_t vertices[] = {0, 7, VERTEX_LOOP[t], VERTEX_LOOP[t+1]};

        // Figure out which of the sixteen possible combinations
        // we're currently experiencing.
        uint8_t lookup = 0;
        for (int v=3; v>=0; --v) {
            lookup = (lookup << 1) + (d[vertices[v]] < 0);
        }

        // Iterate over (up to) two triangles in this tetrahedron
        for (int i=0; i < 2; ++i)
        {
            if (EDGE_MAP[lookup][i][0][0] == -1)    break;

            // Do we need to allocate more space for incoming vertices?
            // If so, double the buffer size.
            if ((*count) + 9 >= (*allocated))
            {
                *allocated *= 2;
                *verts = realloc(*verts, sizeof(float)*(*allocated));
            }

            // ...and insert vertices into the mesh.
            for (int v=0; v < 3; ++v)
            {
                const Vec3f vertex = zero_crossing(d,
                        vertices[EDGE_MAP[lookup][i][v][0]],
                        vertices[EDGE_MAP[lookup][i][v][1]]);
                (*verts)[(*count)++] = vertex.x;
                (*verts)[(*count)++] = vertex.y;
                (*verts)[(*count)++] = vertex.z;
            }
        }
    }

}

// Returns an array of vertices (as x,y,z float triplets).
// Sets *out to the number of vertices returned.
float* triangulate(MathTree* tree, const Region r, unsigned* const out)
{
    float* verts = malloc(9*sizeof(float));
    *out = 0;
    unsigned allocated = 9;

    triangulate_voxel(tree, r, &verts, out, &allocated);
    verts = realloc(verts, (*out)*sizeof(float));
    return verts;
}
