#include <stdlib.h>
#include <stdio.h>

#include "triangulate.h"

#include "tree/tree.h"
#include "tree/eval.h"
#include "util/constants.h"

static const uint8_t VERTEX_LOOP[] = {6, 4, 5, 1, 3, 2, 6};

// Based on which vertices are filled, this map tells you which
// edges to interpolate between when forming zero, one, or two
// triangles for a tetrahedron.
// (filled vertex is first in the pair, and is given as a tetrahedron vertex
//  so you have to translate into a proper cube vertex).
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
        else if (r > EPSILON)   p -= step;
        else                    break;

        step /= 2;
    }

    return (Vec3f){x0 + p*dx, y0 + p*dy, z0 + p*dz};
}

_STATIC_
void get_corner_values(float* d, const Region packed, const Region r, float* data)
{
    for (int i=0; i < 8; ++i)
    {
        // Figure out where this bit of data lives in the larger eval_r array.
        const unsigned index =
            (r.imin - packed.imin + ((i & 4) ? r.ni : 0)) +
            (r.jmin - packed.jmin + ((i & 2) ? r.nj : 0))
                * (packed.ni+1) +
            (r.kmin - packed.kmin + ((i & 1) ? r.nk : 0))
                * (packed.ni+1) * (packed.nj+1);

        d[i] = data[index];
    }
}

_STATIC_
void push_vert(float x, float y, float z,
               float** const verts, unsigned* const count,
               unsigned* const allocated)
{
    if ((*count) + 3 >= (*allocated))
    {
        *allocated *= 2;
        *verts = realloc(*verts, sizeof(float)*(*allocated));
    }

    (*verts)[(*count)++] = x;
    (*verts)[(*count)++] = y;
    (*verts)[(*count)++] = z;
}

_STATIC_
void triangulate_tet(const Region r, float* d, int t,
                     float** const verts, unsigned* const count,
                     unsigned* const allocated)
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

        // ...and insert vertices into the mesh.
        for (int v=0; v < 3; ++v)
        {
            const Vec3f vertex = zero_crossing(d,
                    vertices[EDGE_MAP[lookup][i][v][0]],
                    vertices[EDGE_MAP[lookup][i][v][1]]);
            push_vert(vertex.x * (r.X[1] - r.X[0]) + r.X[0],
                      vertex.y * (r.Y[1] - r.Y[0]) + r.Y[0],
                      vertex.z * (r.Z[1] - r.Z[0]) + r.Z[0],
                      verts, count, allocated);
        }
    }
}


_STATIC_
void triangulate_voxel(MathTree* tree, const Region r,
                       float** const verts, unsigned* const count,
                       unsigned* const allocated,
                       Region packed, const float* data)
{
    // If we can calculate all of the points in this region with a single
    // eval_r call, then do so.  This large chunk will be used in future
    // recursive calls to make things more efficient.
    const unsigned voxels = (r.ni+1)*(r.nj+1)*(r.nk+1);
    if (!data && voxels < MIN_VOLUME)
    {
        packed = r;

        // Copy the X, Y, Z vectors into a flattened matrix form.
        packed.X = malloc(voxels*sizeof(float));
        packed.Y = malloc(voxels*sizeof(float));
        packed.Z = malloc(voxels*sizeof(float));

        int q = 0;
        for (int k = 0; k <= r.nk; ++k) {
            for (int j = 0; j <= r.nj; ++j) {
                for (int i = 0; i <= r.ni; ++i) {
                    packed.X[q] = r.X[i];
                    packed.Y[q] = r.Y[j];
                    packed.Z[q] = r.Z[k];
                    q++;
                }
            }
        }
        // Update the voxel count
        packed.voxels = voxels;

        data = eval_r(tree, packed);

        // Free the allocated matrices
        free(packed.X);
        free(packed.Y);
        free(packed.Z);
    }

    // If we have greater than one voxel, subdivide and recurse.
    if (r.voxels > 1)
    {
        Region octants[8];
        const uint8_t split = octsect(r, octants);
        for (int i=0; i < 8; ++i)
        {
            if (split & (1 << i))
            {
                triangulate_voxel(tree, octants[i],
                                  verts, count, allocated,
                                  packed, data);
            }
        }
        return;
    }

    // Find corner distance values for this voxel
    float d[8];
    get_corner_values(d, packed, r, data);

    // Loop over the six tetrahedra that make up a voxel cell
    for (int t=0; t < 6; ++t)
    {
        triangulate_tet(r, d, t, verts, count, allocated);
    }

}

// Finds an array of vertices (as x,y,z float triplets).
// Sets *count to the number of vertices returned.
void triangulate(MathTree* tree, const Region r,
                 float** const verts, unsigned* const count)
{
    float* v = malloc(9*sizeof(float));
    *count = 0;
    unsigned allocated = 9;

    Region packed;
    triangulate_voxel(tree, r, &v, count, &allocated, packed, NULL);
    *verts = realloc(v, (*count)*sizeof(float));
}

void free_mesh(float* const verts)
{
    free(verts);
}
