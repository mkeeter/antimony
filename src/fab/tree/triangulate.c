#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "triangulate.h"

#include "tree/tree.h"
#include "tree/eval.h"
#include "util/constants.h"
#include "util/vec3f.h"

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

// Node in a linked list of interpolations to run
typedef struct interpolate_command_ {
    enum {INTERPOLATE, CACHED, END_OF_VOXEL} cmd;
    Vec3f v0;
    Vec3f v1;
    unsigned cached;
    struct interpolate_command_* next;
} interpolate_command;

typedef struct {
    // Triangle buffer
    float* verts;
    unsigned count;
    unsigned allocated;

    // Cached region and data from an eval_r call
    Region packed;
    float* data;
    bool has_data;

    // MathTree that we're evaluating
    MathTree* tree;

    // Buffers used for eval_r
    float* X;
    float* Y;
    float* Z;

    // Buffers used in eval_zero_crossings
    float* ex;
    float* ey;
    float* ez;

    // Buffers used in evaluating normals
    float* nx;
    float* ny;
    float* nz;

    // Queue of interpolation commands to be run soon
    interpolate_command* queue;
} tristate;


tristate* tristate_new(MathTree* tree)
{
    tristate* t = (tristate*)malloc(sizeof(tristate));
    *t = (tristate){
        .verts=NULL,
        .count=0,
        .allocated=0,
        .data=malloc(sizeof(float)*MIN_VOLUME),
        .has_data=false,
        .tree=tree,
        .X=malloc(sizeof(float)*MIN_VOLUME),
        .Y=malloc(sizeof(float)*MIN_VOLUME),
        .Z=malloc(sizeof(float)*MIN_VOLUME),
        .ex=malloc(sizeof(float)*MIN_VOLUME),
        .ey=malloc(sizeof(float)*MIN_VOLUME),
        .ez=malloc(sizeof(float)*MIN_VOLUME),
        .nx=malloc(sizeof(float)*MIN_VOLUME),
        .ny=malloc(sizeof(float)*MIN_VOLUME),
        .nz=malloc(sizeof(float)*MIN_VOLUME),
    };
    return t;
}

void tristate_free(tristate* t)
{
    free(t->verts);
    free(t->X);
    free(t->Y);
    free(t->Z);
    free(t->ex);
    free(t->ey);
    free(t->ez);
    free(t->nx);
    free(t->ny);
    free(t->nz);
    free(t->data);
}

// Loads a vertex into the tristate structure.
void tristate_push_vert_(Vec3f v, tristate* t)
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

    (t->verts)[t->count++] = v.x;
    (t->verts)[t->count++] = v.y;
    (t->verts)[t->count++] = v.z;
}

// Returns the normals of the most recent triangle pushed
// to the tristate struct.
void tristate_get_normals(tristate* t, Vec3f* normals)
{
    int c = t->count - 9;
    Region dummy = (Region){
        .X = t->nx,
        .Y = t->ny,
        .Z = t->nz,
        .voxels = 12};

    // Get a small value for epsilon by taking 1/10th the smallest edge length
    // (this will be our sampling step for normal estimation).
    const float len_a = sqrt(pow(t->verts[c] - t->verts[c+3], 2) +
                             pow(t->verts[c+1] - t->verts[c+4], 2) +
                             pow(t->verts[c+2] - t->verts[c+5], 2));
    const float len_b = sqrt(pow(t->verts[c] - t->verts[c+6], 2) +
                             pow(t->verts[c+1] - t->verts[c+7], 2) +
                             pow(t->verts[c+2] - t->verts[c+8], 2));
    const float len_c = sqrt(pow(t->verts[c+3] - t->verts[c+6], 2) +
                             pow(t->verts[c+4] - t->verts[c+7], 2) +
                             pow(t->verts[c+5] - t->verts[c+8], 2));
    const float epsilon = fmin(len_a, fmin(len_b, len_c)) / 10.0f;

    // Load small offset points to estimate normals.
    for (int i=0; i < 3; ++i)
    {
        dummy.X[i*4]     = t->verts[c];
        dummy.X[i*4 + 1] = t->verts[c] + epsilon;
        dummy.X[i*4 + 2] = t->verts[c];
        dummy.X[i*4 + 3] = t->verts[c];
        c++;

        dummy.Y[i*4]     = t->verts[c];
        dummy.Y[i*4 + 1] = t->verts[c];
        dummy.Y[i*4 + 2] = t->verts[c] + epsilon;
        dummy.Y[i*4 + 3] = t->verts[c];
        c++;

        dummy.Z[i*4]     = t->verts[c];
        dummy.Z[i*4 + 1] = t->verts[c];
        dummy.Z[i*4 + 2] = t->verts[c];
        dummy.Z[i*4 + 3] = t->verts[c] + epsilon;
        c++;
    }
    float* out = eval_r(t->tree, dummy);

    // Extract normals from the evaluated data.
    for (int i=0; i < 3; ++i)
    {
        const float dx = out[i*4 + 1] - out[i*4];
        const float dy = out[i*4 + 2] - out[i*4];
        const float dz = out[i*4 + 3] - out[i*4];
        const float len = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
        normals[i] = len == 0 ? (Vec3f){0,0,0} :
            (Vec3f){.x = dx/len, .y = dy/len, .z = dz/len};
    }
}

void tristate_process_feature(tristate* t)
{
    // Get triangle vertices
    const float xa = t->verts[t->count - 9];
    const float ya = t->verts[t->count - 8];
    const float za = t->verts[t->count - 7];
    const float xb = t->verts[t->count - 6];
    const float yb = t->verts[t->count - 5];
    const float zb = t->verts[t->count - 4];
    const float xc = t->verts[t->count - 3];
    const float yc = t->verts[t->count - 2];
    const float zc = t->verts[t->count - 1];

    // Pick out a new center point
    const float xd = (xa + xb + xc) / 3;
    const float yd = (ya + yb + yc) / 3;
    const float zd = (za + zb + zc) / 3;

    t->count -= 9;
    tristate_push_vert_((Vec3f){xa, ya, za}, t);
    tristate_push_vert_((Vec3f){xb, yb, zb}, t);
    tristate_push_vert_((Vec3f){xd, yd, zd}, t);
    // tristate_mark_swappable

    tristate_push_vert_((Vec3f){xb, yb, zb}, t);
    tristate_push_vert_((Vec3f){xc, yc, zc}, t);
    tristate_push_vert_((Vec3f){xd, yd, zd}, t);
    // tristate_mark_swappable

    tristate_push_vert_((Vec3f){xc, yc, zc}, t);
    tristate_push_vert_((Vec3f){xa, ya, za}, t);
    tristate_push_vert_((Vec3f){xd, yd, zd}, t);
    // tristate_mark_swappable
}

void tristate_check_feature(tristate* t)
{
    Vec3f normals[3];
    tristate_get_normals(t, normals);

    // If any of the normals could not be estimated, return immediately.
    for (int i=0; i < 3; ++i)
        if (normals[i].x == 0 && normals[i].y == 0 && normals[i].z == 0)
            return;

    const float ab = vec3f_dot(normals[0], normals[1]);
    const float bc = vec3f_dot(normals[1], normals[2]);
    const float ca = vec3f_dot(normals[2], normals[0]);

    if (ab < 0.95 && bc < 0.95 && ca < 0.95)
    {
        tristate_process_feature(t);
    }
    else if (ab >= 0.95 && bc < 0.95 && ca < 0.95)
    {
        tristate_process_feature(t);
    }
    else if (ab < 0.95 && bc >= 0.95 && ca < 0.95)
    {
        tristate_process_feature(t);
    }
    else if (ab < 0.95 && bc < 0.95 && ca >= 0.95)
    {
        tristate_process_feature(t);
    }
}

// Loads a vertex into the tristate structure.
// If this vertex completes a triangle, check for features.
void tristate_push_vert(Vec3f v, tristate* t)
{
    tristate_push_vert_(v, t);

    if (t->count % 9 == 0)
        tristate_check_feature(t);
}

// Evaluates a region voxel-by-voxel, storing the output in the data
// member of the tristate struct.
void tristate_load_packed(tristate* t, Region r)
{
    // Do a bit of interval arithmetic for tree pruning
    eval_i(t->tree, (Interval){r.X[0], r.X[r.ni]},
                    (Interval){r.Y[0], r.Y[r.nj]},
                    (Interval){r.Z[0], r.Z[r.nk]});
    disable_nodes(t->tree);

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
    memcpy(t->data, eval_r(t->tree, t->packed), voxels * sizeof(float));
    t->has_data = true;
}

void tristate_unload_packed(tristate* t)
{
    enable_nodes(t->tree);
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

void eval_zero_crossings(Vec3f* v0, Vec3f* v1, unsigned count, tristate* t)
{
    float p[count];
    for (unsigned i=0; i < count; ++i)
        p[i] = 0.5;

    float step = 0.25;

    Region dummy = (Region){
        .X = t->ex,
        .Y = t->ey,
        .Z = t->ez,
        .voxels = count};

    for (int iteration=0; iteration < 8; ++iteration)
    {
        // Load new data into the x, y, z arrays.
        for (unsigned i=0; i < count; i++)
        {
            dummy.X[i] = v0[i].x * (1 - p[i]) + v1[i].x * p[i];
            dummy.Y[i] = v0[i].y * (1 - p[i]) + v1[i].y * p[i];
            dummy.Z[i] = v0[i].z * (1 - p[i]) + v1[i].z * p[i];
        }
        float* out = eval_r(t->tree, dummy);

        for (unsigned i=0; i < count; i++)
            if      (out[i] < 0)    p[i] += step;
            else if (out[i] > 0)    p[i] -= step;

        step /= 2;
    }

}

// Flushes out a queue of interpolation commands
void tristate_flush_queue(tristate* t)
{
    Vec3f low[MIN_VOLUME];
    Vec3f high[MIN_VOLUME];

    interpolate_command* list = t->queue;

    // Go through the list, saving a list of vertex pairs on which
    // interpolation should be run into low and high.
    unsigned count=0;
    while (list)
    {
        if (list->cmd == INTERPOLATE)
        {
            low[count] = list->v0;
            high[count] = list->v1;
            count++;
        }
        list = list->next;
    }

    if (count)
        eval_zero_crossings(low, high, count, t);

    // Next, go through and actually load vertices
    // (either directly or from the cache)
    // and delete the linked list.
    count = 0;
    list = t->queue;
    while (list)
    {
        if (list->cmd == INTERPOLATE)
        {
            tristate_push_vert(
                    (Vec3f){t->ex[count], t->ey[count], t->ez[count]}, t);
            count++;
        }
        else if (list->cmd == CACHED)
        {
            unsigned c = list->cached;
            tristate_push_vert((Vec3f){t->ex[c], t->ey[c], t->ez[c]}, t);
        }

        interpolate_command* next = list->next;
        free(list);
        list = next;
    }

    t->queue = NULL;
}

// Push an END_OF_VOXEL command to the command queue.
void tristate_end_voxel(tristate* t)
{
    interpolate_command* cmd = malloc(sizeof(interpolate_command));
    (*cmd) = (interpolate_command){ .cmd=END_OF_VOXEL, .next=NULL };

    interpolate_command** list = &(t->queue);
    while (*list)
        list = &((*list)->next);
    *list = cmd;
}

// Schedule an interpolate calculation in the queue.
void tristate_interpolate_between(tristate* t, Vec3f v0, Vec3f v1)
{
    interpolate_command* cmd = malloc(sizeof(interpolate_command));
    *cmd = (interpolate_command){
        .cmd=INTERPOLATE, .v0=v0, .v1=v1, .next=NULL};


    // Walk through the list, looking for duplicates.
    // If we find the same operation, then switch to a CACHED lookup instead.
    unsigned count = 0;
    interpolate_command** list = &(t->queue);
    while (*list)
    {
        if ((*list)->cmd == INTERPOLATE)
        {
            if ((vec3f_eq(v0, (*list)->v0) && vec3f_eq(v1, (*list)->v1)) ||
                (vec3f_eq(v1, (*list)->v0) && vec3f_eq(v1, (*list)->v0)))
            {
                cmd->cmd = CACHED;
                cmd->cached = count;
            }
            count++;
        }
        list = &((*list)->next);
    }

    (*list) = cmd;
    if (cmd->cmd == INTERPOLATE && (++count) == MIN_VOLUME)
        tristate_flush_queue(t);
}



void tristate_process_tet(const Region r, float* d, int tet,
                          tristate* t)
{
    // Find vertex positions for this tetrahedron
    uint8_t vertices[] = {0, 7, VERTEX_LOOP[tet], VERTEX_LOOP[tet+1]};

    // Figure out which of the sixteen possible combinations
    // we're currently experiencing.
    uint8_t lookup = 0;
    for (int v=3; v>=0; --v)
        lookup = (lookup << 1) + (d[vertices[v]] < 0);

    // Iterate over (up to) two triangles in this tetrahedron
    for (int i=0; i < 2; ++i)
    {
        if (EDGE_MAP[lookup][i][0][0] == -1)
            break;

        // ...and insert vertices into the mesh.
        for (int v=0; v < 3; ++v)
        {
            const uint8_t v0 = vertices[EDGE_MAP[lookup][i][v][0]];
            const uint8_t v1 = vertices[EDGE_MAP[lookup][i][v][1]];

            tristate_interpolate_between(t,
                        (Vec3f){(v0 & 4) ? r.X[1] : r.X[0],
                                (v0 & 2) ? r.Y[1] : r.Y[0],
                                (v0 & 1) ? r.Z[1] : r.Z[0]},
                        (Vec3f){(v1 & 4) ? r.X[1] : r.X[0],
                                (v1 & 2) ? r.Y[1] : r.Y[0],
                                (v1 & 1) ? r.Z[1] : r.Z[0]});
        }
    }
}

void triangulate_region(tristate* t, const Region r)
{
    // If we can calculate all of the points in this region with a single
    // eval_r call, then do so.  This large chunk will be used in future
    // recursive calls to make things more efficient.
    bool loaded_data = !t->has_data;
    if (loaded_data)
        tristate_load_packed(t, r);

    // If we have greater than one voxel, subdivide and recurse.
    if (r.voxels > 1)
    {
        Region octants[8];
        const uint8_t split = octsect(r, octants);
        for (int i=0; i < 8; ++i)
            if (split & (1 << i))
                triangulate_region(t, octants[i]);
    }
    else
    {
        // Load corner values from this voxel
        // (from the packed data array)
        float d[8];
        tristate_get_corner_data(t, r, d);

        // Loop over the six tetrahedra that make up a voxel cell
        for (int tet=0; tet < 6; ++tet)
            tristate_process_tet(r, d, tet, t);
    }

    // Mark that a voxel has ended
    // (which will eventually trigger decimation)
    tristate_end_voxel(t);

    // If this stage of the recursion loaded data into the buffer,
    // clear the has_data flag (so that future stages will re-run
    // eval_r on their portion of the space) and re-enable disabled
    // nodes.
    if (loaded_data)
    {
        tristate_flush_queue(t);
        tristate_unload_packed(t);
    }
}

// Finds an array of vertices (as x,y,z float triplets).
// Sets *count to the number of vertices returned.
void triangulate(MathTree* tree, const Region r,
                 float** const verts, unsigned* const count)
{
    // Make a triangulation state struct.
    tristate* t = tristate_new(tree);

    // Top-level call to the recursive triangulation function.
    triangulate_region(t, r);

    // Copy data from tristate struct to output pointers.
    *verts = malloc(t->count * sizeof(float));
    memcpy(*verts, t->verts, t->count * sizeof(float));
    *count = t->count;

    // Free the triangulation state struct.
    tristate_free(t);
}
