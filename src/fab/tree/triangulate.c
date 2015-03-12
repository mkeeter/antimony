#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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

typedef struct {
    // Triangle buffer
    float* verts;
    unsigned count;
    unsigned allocated;

    // Cached region and data from an eval_r call
    Region packed;
    float* data;
    bool has_data;

    // Buffers used for eval_r
    float* X;
    float* Y;
    float* Z;

    // Buffers used in eval_zero_crossing
    float* x;
    float* y;
    float* z;

    Vec3f cached_zero_crossing[64];
    bool has_cached_zero_crossing[64];
} tristate;


tristate* tristate_new()
{
    tristate* t = (tristate*)malloc(sizeof(tristate));
    *t = (tristate){
        .verts=NULL,
        .count=0,
        .allocated=0,
        .data=malloc(sizeof(float)*MIN_VOLUME),
        .has_data=false,
        .X=malloc(sizeof(float)*MIN_VOLUME),
        .Y=malloc(sizeof(float)*MIN_VOLUME),
        .Z=malloc(sizeof(float)*MIN_VOLUME),
        .x=malloc(sizeof(float)*MIN_VOLUME),
        .y=malloc(sizeof(float)*MIN_VOLUME),
        .z=malloc(sizeof(float)*MIN_VOLUME),
    };
    return t;
}

void tristate_free(tristate* t)
{
    free(t->verts);
    free(t->X);
    free(t->Y);
    free(t->Z);
    free(t->x);
    free(t->y);
    free(t->z);
    free(t->data);
}

void tristate_push_vert(float x, float y, float z, tristate* t)
{
    if (t->allocated == 0)
    {
        t->allocated = 3;
        t->verts = malloc(t->allocated * sizeof(float));
    }
    else if (t->count + 3 >= t->allocated)
    {
        t->allocated *= 2;
        t->verts = realloc(t->verts, sizeof(float)*(t->allocated));
    }

    (t->verts)[t->count++] = x;
    (t->verts)[t->count++] = y;
    (t->verts)[t->count++] = z;
}

void tristate_load_packed(MathTree* tree, tristate* t, Region r)
{
    // Do a bit of interval arithmetic for tree pruning
    eval_i(tree, (Interval){r.X[0], r.X[r.ni]},
                 (Interval){r.Y[0], r.Y[r.nj]},
                 (Interval){r.Z[0], r.Z[r.nk]});
    disable_nodes(tree);

    // Only load the packed matrix if we have few enough voxels.
    const unsigned voxels = (r.ni+1) * (r.nj+1) * (r.nk+1);
    if (voxels >= MIN_VOLUME)
        return;

    // Flatten a 3D region into a 1D list of points that
    // touches every point in the region, one by one.
    int q = 0;
    for (unsigned k=0; k <= r.nk; ++k) {
        for (unsigned j=0; j <= r.nj; ++j) {
            for (unsigned i=0; i <= r.ni; ++i) {
                t->X[q] = r.X[i];
                t->Y[q] = r.Y[j];
                t->Z[q] = r.Z[k];
                q++;
            }
        }
    }

    // Make a dummy region that has the newly-flattened point arrays as the
    // X, Y, Z coordinate data arrays (so that we can run eval_r on it).
    t->packed = (Region) {
        .imin=r.imin, .jmin=r.jmin, .kmin=r.kmin,
        .ni=r.ni, .nj=r.nj, .nk=r.nk,
        .X=t->X, .Y=t->Y, .Z=t->Z, .voxels=voxels};

    // Run eval_r and copy the data out
    memcpy(t->data, eval_r(tree, t->packed), voxels * sizeof(float));
    t->has_data = true;
}

void tristate_unload_packed(tristate* t, MathTree* tree)
{
    enable_nodes(tree);
    t->has_data = false;
}

void tristate_get_corner_data(tristate* t, const Region r, float d[8])
{
    // Populates an 8-element array with the function evaluation
    // results from the corner of a single-voxel region.
    for (int i=0; i < 8; ++i)
    {
        // Figure out where this bit of data lives in the larger eval_r array.
        const unsigned index =
            (r.imin - t->packed.imin + ((i & 4) ? r.ni : 0)) +
            (r.jmin - t->packed.jmin + ((i & 2) ? r.nj : 0))
                * (t->packed.ni+1) +
            (r.kmin - t->packed.kmin + ((i & 1) ? r.nk : 0))
                * (t->packed.ni+1) * (t->packed.nj+1);

        d[i] = t->data[index];
    }
}

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

bool tristate_has_cached_zero_crossing(tristate* t, uint8_t v0, uint8_t v1)
{
    return t->has_cached_zero_crossing[(v0 << 3) | v1];
}

Vec3f tristate_cached_zero_crossing(tristate* t, uint8_t v0, uint8_t v1)
{
    return t->cached_zero_crossing[(v0 << 3) | v1];
}

Vec3f tristate_cache_zero_crossing(tristate* t, uint8_t v0, uint8_t v1, Vec3f v)
{
    t->cached_zero_crossing[(v0 << 3) | v1] = v;
    t->cached_zero_crossing[(v1 << 3) | v0] = v;
    t->has_cached_zero_crossing[(v0 << 3) | v1] = true;
    t->has_cached_zero_crossing[(v1 << 3) | v0] = true;
}

void tristate_clear_zero_crossing_cache(tristate* t)
{
    for (int i=0; i < 64; i++)
        t->has_cached_zero_crossing[i] = false;
}

Vec3f eval_zero_crossing(Vec3f v0, Vec3f v1, MathTree* tree, tristate* t)
{
    float p = 0.5;
    float step = 0.25;

    for (int iteration=0; iteration < 4; ++iteration)
    {
        const float r = eval_f(tree,
            v0.x * (1 - p) + v1.x * p,
            v0.y * (1 - p) + v1.y * p,
            v0.z * (1 - p) + v1.z * p);

        if (r < 0)          p += step;
        else if (r > 0)     p -= step;
        else                break;

        step /= 2;
    }
    return (Vec3f){v0.x * (1 - p) + v1.x * p,
                   v0.y * (1 - p) + v1.y * p,
                   v0.z * (1 - p) + v1.z * p};
}

Vec3f eval_zero_crossing_(Vec3f v0, Vec3f v1, MathTree* tree, tristate* t)
{
    for (int i=0; i < MIN_VOLUME; ++i)
    {
        float f = i / (float)(MIN_VOLUME - 1);
        t->x[i] = v0.x * (1 - f) + v1.x * f;
        t->y[i] = v0.y * (1 - f) + v1.y * f;
        t->z[i] = v0.z * (1 - f) + v1.z * f;
    }

    Region r = (Region){
        .X = t->x,
        .Y = t->y,
        .Z = t->z,
        .voxels = MIN_VOLUME
    };

    float* out = eval_r(tree, r);

    for (int i=0; i < MIN_VOLUME; i++)
        if (out[i] >= 0)
            return (Vec3f){t->x[i], t->y[i], t->z[i]};

    printf("Zero crossing detection failed!\n");
    return (Vec3f){(v0.x + v1.x) / 2,
                   (v0.y + v1.y) / 2,
                   (v0.z + v1.z) / 2};
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

void tristate_process_tet(const Region r, float* d, int tet,
                          MathTree* tree, tristate* t)
{
    // Find vertex positions for this tetrahedron
    uint8_t vertices[] = {0, 7, VERTEX_LOOP[tet], VERTEX_LOOP[tet+1]};

    // Figure out which of the sixteen possible combinations
    // we're currently experiencing.
    uint8_t lookup = 0;
    for (int v=3; v>=0; --v)
        lookup = (lookup << 1) + (d[vertices[v]] < 0);

    // Clear the cache of zero crossing locations
    tristate_clear_zero_crossing_cache(t);

    // Iterate over (up to) two triangles in this tetrahedron
    for (int i=0; i < 2; ++i)
    {
        if (EDGE_MAP[lookup][i][0][0] == -1)
            break;

        // ...and insert vertices into the mesh.
        for (int v=0; v < 3; ++v)
        {
#if 1
            const uint8_t v0 = vertices[EDGE_MAP[lookup][i][v][0]];
            const uint8_t v1 = vertices[EDGE_MAP[lookup][i][v][1]];

            Vec3f vertex_exact;
            if (tristate_has_cached_zero_crossing(t, v0, v1))
            {
                vertex_exact = tristate_cached_zero_crossing(t, v0, v1);
            }
            else
            {
                vertex_exact = eval_zero_crossing(
                        (Vec3f){(v0 & 4) ? r.X[1] : r.X[0],
                                (v0 & 2) ? r.Y[1] : r.Y[0],
                                (v0 & 1) ? r.Z[1] : r.Z[0]},
                        (Vec3f){(v1 & 4) ? r.X[1] : r.X[0],
                                (v1 & 2) ? r.Y[1] : r.Y[0],
                                (v1 & 1) ? r.Z[1] : r.Z[0]},
                        tree, t);
                tristate_cache_zero_crossing(t, v0, v1, vertex_exact);
            }

            tristate_push_vert(vertex_exact.x, vertex_exact.y, vertex_exact.z, t);
#else
            const Vec3f vertex = zero_crossing(d,
                    vertices[EDGE_MAP[lookup][i][v][0]],
                    vertices[EDGE_MAP[lookup][i][v][1]]);
            tristate_push_vert(vertex.x * (r.X[1] - r.X[0]) + r.X[0],
                               vertex.y * (r.Y[1] - r.Y[0]) + r.Y[0],
                               vertex.z * (r.Z[1] - r.Z[0]) + r.Z[0], t);
#endif
        }
    }
}

void triangulate_region(tristate* t, MathTree* tree, const Region r)
{
    // If we can calculate all of the points in this region with a single
    // eval_r call, then do so.  This large chunk will be used in future
    // recursive calls to make things more efficient.
    bool loaded_data = !t->has_data;
    if (loaded_data)
        tristate_load_packed(tree, t, r);

    // If we have greater than one voxel, subdivide and recurse.
    if (r.voxels > 1)
    {
        Region octants[8];
        const uint8_t split = octsect(r, octants);
        for (int i=0; i < 8; ++i)
            if (split & (1 << i))
                triangulate_region(t, tree, octants[i]);
    }
    else
    {
        // Load corner values from this voxel
        // (from the packed data array)
        float d[8];
        tristate_get_corner_data(t, r, d);

        // Loop over the six tetrahedra that make up a voxel cell
        for (int tet=0; tet < 6; ++tet)
            tristate_process_tet(r, d, tet, tree, t);
    }

    // If this stage of the recursion loaded data into the buffer,
    // clear the has_data flag (so that future stages will re-run
    // eval_r on their portion of the space) and re-enable disabled
    // nodes.
    if (loaded_data)
        tristate_unload_packed(t, tree);
}

// Finds an array of vertices (as x,y,z float triplets).
// Sets *count to the number of vertices returned.
void triangulate(MathTree* tree, const Region r,
                 float** const verts, unsigned* const count)
{
    // Make a triangulation state struct.
    tristate* t = tristate_new();

    // Top-level call to the recursive triangulation function.
    triangulate_region(t, tree, r);

    // Copy data from tristate struct to output pointers.
    *verts = malloc(t->count * sizeof(float));
    memcpy(*verts, t->verts, t->count * sizeof(float));
    *count = t->count;

    // Free the triangulation state struct.
    tristate_free(t);
}
