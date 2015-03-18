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

#include "Eigen/Dense"

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


Mesher::Mesher(MathTree* tree)
    : tree(tree), data(new float[MIN_VOLUME]), has_data(false),
      X(new float[MIN_VOLUME]),
      Y(new float[MIN_VOLUME]),
      Z(new float[MIN_VOLUME]),
      ex(new float[MIN_VOLUME]),
      ey(new float[MIN_VOLUME]),
      ez(new float[MIN_VOLUME]),
      nx(new float[MIN_VOLUME]),
      ny(new float[MIN_VOLUME]),
      nz(new float[MIN_VOLUME])
{
    // Nothing to do here
}

Mesher::~Mesher()
{
    for (auto ptr : {data, X, Y, Z, ex, ey, ez, nx, ny, nz})
        delete [] ptr;
}


// Returns the normals of the most recent triangle pushed
// to the tristate struct.
void Mesher::get_normals(Vec3f* normals)
{
   int c = verts.size()- 9;
    Region dummy = (Region){
        .X = nx, .Y = ny, .Z = nz,
        .voxels = 12};

    // Get a small value for epsilon by taking 1/10th the smallest edge length
    // (this will be our sampling step for normal estimation).
    const float len_a = sqrt(pow(verts[c] - verts[c+3], 2) +
                             pow(verts[c+1] - verts[c+4], 2) +
                             pow(verts[c+2] - verts[c+5], 2));
    const float len_b = sqrt(pow(verts[c] - verts[c+6], 2) +
                             pow(verts[c+1] - verts[c+7], 2) +
                             pow(verts[c+2] - verts[c+8], 2));
    const float len_c = sqrt(pow(verts[c+3] - verts[c+6], 2) +
                             pow(verts[c+4] - verts[c+7], 2) +
                             pow(verts[c+5] - verts[c+8], 2));
    const float epsilon = fmin(len_a, fmin(len_b, len_c)) / 10.0f;

    // Load small offset points to estimate normals.
    for (int i=0; i < 3; ++i)
    {
        dummy.X[i*4]     = verts[c];
        dummy.X[i*4 + 1] = verts[c] + epsilon;
        dummy.X[i*4 + 2] = verts[c];
        dummy.X[i*4 + 3] = verts[c];
        c++;

        dummy.Y[i*4]     = verts[c];
        dummy.Y[i*4 + 1] = verts[c];
        dummy.Y[i*4 + 2] = verts[c] + epsilon;
        dummy.Y[i*4 + 3] = verts[c];
        c++;

        dummy.Z[i*4]     = verts[c];
        dummy.Z[i*4 + 1] = verts[c];
        dummy.Z[i*4 + 2] = verts[c];
        dummy.Z[i*4 + 3] = verts[c] + epsilon;
        c++;
    }
    float* out = eval_r(tree, dummy);

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

// Mark that the first edge of the most recent triangle is swappable
// (as part of feature detection / extraction).
void Mesher::mark_swappable()
{
    const unsigned c = verts.size();
    std::array<float, 6> key = {{
        verts[c-9], verts[c-8], verts[c-7],
        verts[c-6], verts[c-5], verts[c-4]}};

    auto found = swappable.find(key);
    if (found != swappable.end())
    {
        const unsigned a = found->second;
        const unsigned b = c - 9;

        for (int i=0; i < 3; ++i)
        {
            verts[a + 3 + i] = verts[b + 6 + i];
            verts[b + 3 + i] = verts[a + 6 + i];
        }
    }
    else
    {
        std::array<float, 6> reversed = {{
            verts[c-6], verts[c-5], verts[c-4],
            verts[c-9], verts[c-8], verts[c-7]}};
        swappable[reversed] = c - 9;
    }
}

Vec3f Mesher::edge_feature_point(const Vec3f& a, const Vec3f& na,
                                 const Vec3f& b, const Vec3f& nb,
                                 const Vec3f& c)
{
    // Find two combined edge normals
    Vec3f n_ab = edge_normal(a, b, c);
    Vec3f n_bc = edge_normal(b, c, a);

    Vec3f p0 = plane_intersection(a, na, b, nb, a, n_ab);
    Vec3f p1 = plane_intersection(a, na, b, nb, b, n_bc);

    return (Vec3f){(p0.x + p1.x)/2, (p0.y + p1.y)/2, (p0.z + p1.z)/2};
}

void Mesher::process_feature(Mesher::FeatureType t, Vec3f* normals)
{
    // Get triangle vertices
    unsigned c = verts.size();
    const float xa = verts[c - 9];
    const float ya = verts[c - 8];
    const float za = verts[c - 7];
    const float xb = verts[c - 6];
    const float yb = verts[c - 5];
    const float zb = verts[c - 4];
    const float xc = verts[c - 3];
    const float yc = verts[c - 2];
    const float zc = verts[c - 1];


    // Pick out a new center point
    Vec3f intersection = (Vec3f){
        (xa + xb + xc) / 3,
        (ya + yb + yc) / 3,
        (za + zb + zc) / 3};

    if (t == FEATURE_CORNER)
    {
        intersection = plane_intersection(
                (Vec3f){xa, ya, za}, normals[0],
                (Vec3f){xb, yb, zb}, normals[1],
                (Vec3f){xc, yc, zc}, normals[2]);
    }
    else if (t == FEATURE_EDGE_AB_C)
    {
        intersection = edge_feature_point(
                (Vec3f){xb, yb, zb}, normals[1],
                (Vec3f){xc, yc, zc}, normals[2],
                (Vec3f){xa, ya, za});
    }
    else if (t == FEATURE_EDGE_BC_A)
    {
        intersection = edge_feature_point(
                (Vec3f){xc, yc, zc}, normals[2],
                (Vec3f){xa, ya, za}, normals[0],
                (Vec3f){xb, yb, zb});
    }
    else if (t == FEATURE_EDGE_CA_B)
    {
        intersection = edge_feature_point(
                (Vec3f){xa, ya, za}, normals[0],
                (Vec3f){xb, yb, zb}, normals[1],
                (Vec3f){xc, yc, zc});
    }

    const float xd = intersection.x;
    const float yd = intersection.y;
    const float zd = intersection.z;

    for (int i=0; i < 9; ++i)
        verts.pop_back();

    // Add the new (split) triangles
    for (auto i : {xa, ya, za, xb, yb, zb, xd, yd, zd})
        verts.push_back(i);
    if (t != FEATURE_EDGE_AB_C)
        mark_swappable();

    for (auto i : {xb, yb, zb, xc, yc, zc, xd, yd, zd})
        verts.push_back(i);
    if (t != FEATURE_EDGE_BC_A)
        mark_swappable();

    for (auto i : {xc, yc, zc, xa, ya, za, xd, yd, zd})
        verts.push_back(i);
    if (t != FEATURE_EDGE_CA_B)
        mark_swappable();
}

void Mesher::check_feature()
{
    Vec3f normals[3];
    get_normals(normals);

    // If any of the normals could not be estimated, return immediately.
    for (int i=0; i < 3; ++i)
        if (normals[i].x == 0 && normals[i].y == 0 && normals[i].z == 0)
            return;

    const float ab = vec3f_dot(normals[0], normals[1]);
    const float bc = vec3f_dot(normals[1], normals[2]);
    const float ca = vec3f_dot(normals[2], normals[0]);

    if (ab < 0.95 && bc < 0.95 && ca < 0.95)
    {
        process_feature(FEATURE_CORNER, normals);
    }
    else if (ab >= 0.95 && bc < 0.95 && ca < 0.95)
    {
        process_feature(FEATURE_EDGE_AB_C, normals);
    }
    else if (ab < 0.95 && bc >= 0.95 && ca < 0.95)
    {
        process_feature(FEATURE_EDGE_BC_A, normals);
    }
    else if (ab < 0.95 && bc < 0.95 && ca >= 0.95)
    {
        process_feature(FEATURE_EDGE_CA_B, normals);
    }
}

// Loads a vertex into the vertex list.
// If this vertex completes a triangle, check for features.
void Mesher::push_vert(const Vec3f& v)
{
    verts.push_back(v.x);
    verts.push_back(v.y);
    verts.push_back(v.z);

    if (verts.size() % 9 == 0)
        check_feature();
}

// Evaluates a region voxel-by-voxel, storing the output in the data
// member of the tristate struct.
void Mesher::load_packed(const Region& r)
{
    // Do a round of interval evaluation for tree pruning
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
                X[q] = r.X[i];
                Y[q] = r.Y[j];
                Z[q] = r.Z[k];
                q++;
            }
        }
    }

    // Make a dummy region that has the newly-flattened point arrays as the
    // X, Y, Z coordinate data arrays (so that we can run eval_r on it).
    packed = (Region) {
        .imin=r.imin, .jmin=r.jmin, .kmin=r.kmin,
        .ni=r.ni, .nj=r.nj, .nk=r.nk,
        .X=X, .Y=Y, .Z=Z, .voxels=voxels};

    // Run eval_r and copy the data out
    memcpy(data, eval_r(tree, packed), voxels * sizeof(float));
    has_data = true;
}

void Mesher::unload_packed()
{
    enable_nodes(tree);
    has_data = false;
}

void Mesher::get_corner_data(const Region& r, float d[8])
{
    // Populates an 8-element array with the function evaluation
    // results from the corner of a single-voxel region.
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

void Mesher::eval_zero_crossings(Vec3f* v0, Vec3f* v1, unsigned count)
{
    float p[count];
    for (unsigned i=0; i < count; ++i)
        p[i] = 0.5;

    float step = 0.25;

    Region dummy = (Region){
        .X = ex,
        .Y = ey,
        .Z = ez,
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
        float* out = eval_r(tree, dummy);

        for (unsigned i=0; i < count; i++)
            if      (out[i] < 0)    p[i] += step;
            else if (out[i] > 0)    p[i] -= step;

        step /= 2;
    }

}

// Flushes out a queue of interpolation commands
void Mesher::flush_queue()
{
    Vec3f low[MIN_VOLUME];
    Vec3f high[MIN_VOLUME];

    // Go through the list, saving a list of vertex pairs on which
    // interpolation should be run into low and high.
    unsigned count=0;
    for (auto c : queue)
    {
        if (c.cmd == InterpolateCommand::INTERPOLATE)
        {
            low[count] = c.v0;
            high[count] = c.v1;
            count++;
        }
    }

    if (count)
        eval_zero_crossings(low, high, count);

    // Next, go through and actually load vertices
    // (either directly or from the cache)
    count = 0;
    for (auto c : queue)
    {
        if (c.cmd == InterpolateCommand::INTERPOLATE)
        {
            push_vert((Vec3f){ex[count], ey[count], ez[count]});
            count++;
        }
        else if (c.cmd == InterpolateCommand::CACHED)
        {
            unsigned i = c.cached;
            push_vert((Vec3f){ex[i], ey[i], ez[i]});
        }
    }
    queue.clear();
}

// Push an END_OF_VOXEL command to the command queue.
void Mesher::end_voxel()
{
    queue.push_back((InterpolateCommand){
            .cmd=InterpolateCommand::END_OF_VOXEL});
}

// Schedule an interpolate calculation in the queue.
void Mesher::interpolate_between(const Vec3f& v0, const Vec3f& v1)
{
    InterpolateCommand next = (InterpolateCommand){
        .cmd=InterpolateCommand::INTERPOLATE, .v0=v0, .v1=v1};

    // Walk through the list, looking for duplicates.
    // If we find the same operation, then switch to a CACHED lookup instead.
    unsigned count = 0;
    for (auto c : queue)
    {
        if (c.cmd == InterpolateCommand::INTERPOLATE)
        {
            if ((vec3f_eq(v0, c.v0) && vec3f_eq(v1, c.v1)) ||
                (vec3f_eq(v1, c.v0) && vec3f_eq(v1, c.v0)))
            {
                next.cmd = InterpolateCommand::CACHED;
                next.cached = count;
            }
            count++;
        }
    }

    queue.push_back(next);
    if (next.cmd == InterpolateCommand::INTERPOLATE &&
            queue.size() == MIN_VOLUME)
        flush_queue();
}



void Mesher::process_tet(const Region& r, const float* const d, const int tet)
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

            interpolate_between(
                        (Vec3f){(v0 & 4) ? r.X[1] : r.X[0],
                                (v0 & 2) ? r.Y[1] : r.Y[0],
                                (v0 & 1) ? r.Z[1] : r.Z[0]},
                        (Vec3f){(v1 & 4) ? r.X[1] : r.X[0],
                                (v1 & 2) ? r.Y[1] : r.Y[0],
                                (v1 & 1) ? r.Z[1] : r.Z[0]});
        }
    }
}

Vec3f Mesher::plane_intersection(const Vec3f& pa_, const Vec3f& na_,
                                 const Vec3f& pb_, const Vec3f& nb_,
                                 const Vec3f& pc_, const Vec3f& nc_)
{
    Eigen::Vector3d pa(pa_.x, pa_.y, pa_.z),
                    na(na_.x, na_.y, na_.z),
                    pb(pb_.x, pb_.y, pb_.z),
                    nb(nb_.x, nb_.y, nb_.z),
                    pc(pc_.x, pc_.y, pc_.z),
                    nc(nc_.x, nc_.y, nc_.z);

    Eigen::Matrix3d n;
    n.col(0) << na;
    n.col(1) << nb;
    n.col(2) << nc;

    // Graphics Gems 1
    // Intersection of Three Planes
    // Ronald Goldman
    Eigen::Vector3d out =
        (pa.dot(na)*nb.cross(nc) +
         pb.dot(nb)*nc.cross(na) +
         pc.dot(nc)*na.cross(nb)) / n.determinant();

    return (Vec3f){static_cast<float>(out[0]),
                   static_cast<float>(out[1]),
                   static_cast<float>(out[2])};
}

Vec3f Mesher::edge_normal(const Vec3f& a_, const Vec3f& b_, const Vec3f& c_)
{
    Eigen::Vector3d a(a_.x, a_.y, a_.z),
                    b(b_.x, b_.y, b_.z),
                    c(c_.x, c_.y, c_.z);

    Eigen::Vector3d out = (b-a).cross(c-a).cross(b-a);

    return (Vec3f){static_cast<float>(out[0]),
                   static_cast<float>(out[1]),
                   static_cast<float>(out[2])};
}

void Mesher::triangulate_region(const Region& r)
{
    // If we can calculate all of the points in this region with a single
    // eval_r call, then do so.  This large chunk will be used in future
    // recursive calls to make things more efficient.
    bool loaded_data = !has_data;
    if (loaded_data)
        load_packed(r);

    // If we have greater than one voxel, subdivide and recurse.
    if (r.voxels > 1)
    {
        Region octants[8];
        const uint8_t split = octsect(r, octants);
        for (int i=0; i < 8; ++i)
            if (split & (1 << i))
                triangulate_region(octants[i]);
    }
    else
    {
        // Load corner values from this voxel
        // (from the packed data array)
        float d[8];
        get_corner_data(r, d);

        // Loop over the six tetrahedra that make up a voxel cell
        for (int tet=0; tet < 6; ++tet)
            process_tet(r, d, tet);
    }

    // Mark that a voxel has ended
    // (which will eventually trigger decimation)
    end_voxel();

    // If this stage of the recursion loaded data into the buffer,
    // clear the has_data flag (so that future stages will re-run
    // eval_r on their portion of the space) and re-enable disabled
    // nodes.
    if (loaded_data)
    {
        flush_queue();
        unload_packed();
    }
}

// Finds an array of vertices (as x,y,z float triplets).
// Sets *count to the number of vertices returned.
void triangulate(MathTree* tree, const Region r,
                 float** const verts, unsigned* const count)
{
    Mesher t(tree);

    // Top-level call to the recursive triangulation function.
    t.triangulate_region(r);

    // Copy data from tristate struct to output pointers.
    *verts = (float*)malloc(t.verts.size() * sizeof(float));
    memcpy(*verts, t.verts.data(), t.verts.size() * sizeof(float));
    *count = t.verts.size();
}
