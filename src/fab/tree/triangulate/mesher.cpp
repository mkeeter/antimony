#include "tree/triangulate/mesher.h"

#include "tree/tree.h"
#include "tree/eval.h"
#include "util/switches.h"

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


Mesher::Mesher(MathTree* tree, bool detect_edges)
    : tree(tree), detect_edges(detect_edges),
      data(new float[MIN_VOLUME]), has_data(false),
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
Triangle Mesher::get_normals(const Triangle& t)
{
    // Get a small value for epsilon by taking 1/10th the smallest edge length
    // (this will be our sampling step for normal estimation).
    const float len_a = (t.a - t.b).norm();
    const float len_b = (t.b - t.c).norm();
    const float len_c = (t.c - t.a).norm();
    const float epsilon = fmin(len_a, fmin(len_b, len_c)) / 10.0f;

    // We'll be evaluating a dummy region to numerically estimate gradients
    Region dummy = (Region){
        .X = nx, .Y = ny, .Z = nz,
        .voxels = 12};

    // Load position data into the dummy region
    int i=0;
    for (auto v : {t.a, t.b, t.c})
    {
        dummy.X[i]   = v[0];
        dummy.X[i+1] = v[0] + epsilon;
        dummy.X[i+2] = v[0];
        dummy.X[i+3] = v[0];

        dummy.Y[i]   = v[1];
        dummy.Y[i+1] = v[1];
        dummy.Y[i+2] = v[1] + epsilon;
        dummy.Y[i+3] = v[1];

        dummy.Z[i]   = v[2];
        dummy.Z[i+1] = v[2];
        dummy.Z[i+2] = v[2];
        dummy.Z[i+3] = v[2] + epsilon;
        i += 4;
    }

    float* out = eval_r(tree, dummy);

    // Extract normals from the evaluated data.
    Triangle normals;
    i = 0;
    for (auto n : {&normals.a, &normals.b, &normals.c})
    {
        const float dx = out[i+1] - out[i];
        const float dy = out[i+2] - out[i];
        const float dz = out[i+3] - out[i];
        *n = Vec3f(dx, dy, dz).normalized();
        i += 4;
    }

    return normals;
}

// Mark that the first edge of the most recent triangle is swappable
// (as part of feature detection / extraction).
void Mesher::mark_swappable()
{
    /*
    const unsigned c = verts.size();
    std::array<float, 6> key = {{
        verts[c-9], verts[c-8], verts[c-7],
        verts[c-6], verts[c-5], verts[c-4]}};

    auto found = swappable.find(key);
    if (found != swappable.end())
    {
        const unsigned a = found->second;
        const unsigned b = c - 9;

        // If the new triangle is overlapping with its neighbor, then
        // merge the two instead of letting them overlap.
        const Vec3f e = (Vec3f){verts[a+3] - verts[a],
                                verts[a+4] - verts[a+1],
                                verts[a+5] - verts[a+2]};
        const float dot = vec3f_dot(e,
                vec3f_cross(get_triangle_normal(a),
                            get_triangle_normal(b)));

        if (dot >= 0)
        {
            Vec3f swap = (Vec3f){verts[a+6], verts[a+7], verts[a+8]};
            // Loop through and find other instances of this vertex,
            // then replace them with the new vertex.
            for (int i=a-18; i > a+27; i += 3)
            {
                if (i < 0 || i + 2 >= verts.size())
                    continue;

                if (verts[i] == verts[a+6] &&
                    verts[i+1] == verts[a+7] &&
                    verts[i+2] == verts[a+8])
                {
                    verts[i] = verts[b+6];
                    verts[i+1] = verts[b+7];
                    verts[i+2] = verts[b+8];
                }
            }
            removed.push_back(a);
        }
        else
        {
            for (int i=0; i < 3; ++i)
            {
                verts[a + 3 + i] = verts[b + 6 + i];
                verts[b + 3 + i] = verts[a + 6 + i];
            }
        }
        swappable.erase(found);
    }
    else
    {
        std::array<float, 6> reversed = {{
            verts[c-6], verts[c-5], verts[c-4],
            verts[c-9], verts[c-8], verts[c-7]}};
        swappable[reversed] = c - 9;
    }
    */
}

Vec3f Mesher::plane_plane_point(const Vec3f& a, const Vec3f& na,
                                const Vec3f& b, const Vec3f& nb,
                                const Vec3f& c)
{
    /*
    // Convert into Hessian normal form
    const float pa = -vec3f_dot(na, a);
    const float pb = -vec3f_dot(nb, b);

    // Finding the edge direction is easy:
    const Vec3f edge = vec3f_cross(na, nb);

    // Finding a point on the edge is a bit tricker.
    // We collapse the undetermined system by setting x, y, or z=0
    // (depending on the edge's normal).
    float eq1[2];
    float eq2[2];
    char zeroed;
    if (fabs(edge.x) > 0.01)
    {
        eq1[0] = na.y; eq1[1] = na.z;
        eq2[0] = nb.y; eq2[1] = nb.z;
        zeroed = 'x';
    }
    else if (fabs(edge.y) > 0.01)
    {
        eq1[0] = na.x; eq1[1] = na.z;
        eq2[0] = nb.x; eq2[1] = nb.z;
        zeroed = 'y';
    } else {
        eq1[0] = na.x; eq1[1] = na.y;
        eq2[0] = nb.x; eq2[1] = nb.y;
        zeroed = 'z';
    }

    // Then, solve the pair of simultaneous equations
    const float z = (-pa + eq1[0]/eq2[0]*pb) /
                    ((eq1[1] - eq1[0]/eq2[0]) * eq2[1]);
    const float y = (-pa - eq1[1]*z) / eq1[0];

    // Lastly, create the point at the right place, depending on which
    // term had been zeroed to collapse the underdetermined system.
    Vec3f edge_pt;
    if (zeroed == 'x')
        edge_pt = (Vec3f){0, y, z};
    else if (zeroed == 'y')
        edge_pt = (Vec3f){y, 0, z};
    else
        edge_pt = (Vec3f){y, z, 0};

    // Lastly, work out the point on the edge that is the closest to the
    // provided point c.
    const float t = vec3f_dot(edge, (Vec3f){c.x - edge_pt.x,
                                            c.y - edge_pt.y,
                                            c.z - edge_pt.z});
    return (Vec3f){edge_pt.x + t * edge.x,
                   edge_pt.y + t * edge.y,
                   edge_pt.z + t * edge.z};
                   */
}

Vec3f Mesher::edge_feature_point(const Vec3f& a, const Vec3f& na,
                                 const Vec3f& b, const Vec3f& nb,
                                 const Vec3f& c, const Vec3f& nc,
                                 const Vec3f& fallback)
{
    // Find two combined edge normals
    Vec3f n_ab = edge_normal(a, b, c);
    Vec3f n_bc = edge_normal(b, c, a);

    Vec3f center = (a + b + c) / 3;

    Vec3f p0 = plane_intersection(a, na, b, nb, center, n_ab, fallback);
    Vec3f p1 = plane_intersection(c, nc, b, nb, center, n_bc, fallback);

    Vec3f d0 = p0 - center;
    Vec3f d1 = p1 - center;

    return d0.norm() < d1.norm() ? p0 : p1;
}

bool Mesher::point_in_triangle(const Vec3f& a, const Vec3f& b,
                               const Vec3f& c, const Vec3f& p)
{
    // Triangle's normal
    Eigen::Vector3d n = (b - a).cross(c - a).normalized();

    // Projected point
    Eigen::Vector3d proj = p - n*(p - a).dot(n);

    // Check that the point is on the correct side of the three
    // lines in the triangle; returning false otherwise.
    if ((proj - a).cross(c - a).dot(n) <= 0)
        return false;
    if ((proj - b).cross(a - b).dot(n) <= 0)
        return false;
    if ((proj - c).cross(b - c).dot(n) <= 0)
        return false;
    return true;
}

bool Mesher::process_feature(Mesher::FeatureType type, const Triangle& tri,
                             const Triangle& normals)
{
    /*
    // Pick out a new center point
    Vec3f intersection = (tri.a + tri.b + tri.c) / 3;

    if (type == FEATURE_CORNER)
    {
        intersection = plane_intersection(
                (Vec3f){xa, ya, za}, normals[0],
                (Vec3f){xb, yb, zb}, normals[1],
                (Vec3f){xc, yc, zc}, normals[2], intersection);

        if (!point_in_triangle((Vec3f){xa, ya, za},
                               (Vec3f){xb, yb, zb},
                               (Vec3f){xc, yc, zc}, intersection))
        {
            return false;
        }
    }
    else if (type == FEATURE_EDGE_AB_C)
    {
        intersection = edge_feature_point(
                (Vec3f){xb, yb, zb}, normals[1],
                (Vec3f){xc, yc, zc}, normals[2],
                (Vec3f){xa, ya, za}, normals[0], intersection);
    }
    else if (type == FEATURE_EDGE_BC_A)
    {
        intersection = edge_feature_point(
                (Vec3f){xc, yc, zc}, normals[2],
                (Vec3f){xa, ya, za}, normals[0],
                (Vec3f){xb, yb, zb}, normals[1], intersection);
    }
    else if (type == FEATURE_EDGE_CA_B)
    {
        intersection = edge_feature_point(
                (Vec3f){xa, ya, za}, normals[0],
                (Vec3f){xb, yb, zb}, normals[1],
                (Vec3f){xc, yc, zc}, normals[2], intersection);
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

        */
    return true;
}

void Mesher::add_triangle(const Triangle& t)
{
    // Skip edge detection if it is turned off.
    if (!detect_edges)
    {
        triangles.push_back(t);
        return;
    }

    // If any of the normals could not be estimated, return immediately.
    Triangle normals = get_normals(t);
    for (auto v : {normals.a, normals.b, normals.c})
        if (v[0] == 0 && v[1] == 0 && v[2] == 0)
        {
            triangles.push_back(t);
            return;
        }

    const float ab = fabs(normals.a.dot(normals.b));
    const float bc = fabs(normals.b.dot(normals.c));
    const float ca = fabs(normals.c.dot(normals.a));

    // This is threshold for deciding that two normals are the same
    // (when they are dot-producted together)
    const float same = 0.95;

    if (ab < same && bc < same && ca < same)
    {
        // If adding the point for the corner feature failed,
        // then pick the best-fitting edge and use it instead.
        if (!process_feature(FEATURE_CORNER, t, normals))
        {
            if (ab >= bc && ab >= ca)
                process_feature(FEATURE_EDGE_AB_C, t, normals);
            else if (bc >= ab && bc >= ca)
                process_feature(FEATURE_EDGE_BC_A, t, normals);
            else
                process_feature(FEATURE_EDGE_CA_B, t, normals);
        }
    }
    else if (ab >= same && bc < same && ca < same)
    {
        process_feature(FEATURE_EDGE_AB_C, t, normals);
    }
    else if (ab < same && bc >= same && ca < same)
    {
        process_feature(FEATURE_EDGE_BC_A, t, normals);
    }
    else if (ab < same && bc < same && ca >= same)
    {
        process_feature(FEATURE_EDGE_CA_B, t, normals);
    }
    else
    {
        triangles.push_back(t);
    }
}

// Loads a vertex into the vertex list.
// If this vertex completes a triangle, check for features.
void Mesher::push_vert(const float x, const float y, const float z)
{
    triangle.push_back(Vec3f(x, y, z));
    if (triangle.size() == 3)
    {
        add_triangle(Triangle(triangle[0], triangle[1], triangle[2]));
        triangle.clear();
    }
}

// Evaluates a region voxel-by-voxel, storing the output in the data
// member of the tristate struct.
bool Mesher::load_packed(const Region& r)
{
    // Only load the packed matrix if we have few enough voxels.
    const unsigned voxels = (r.ni+1) * (r.nj+1) * (r.nk+1);
    if (voxels >= MIN_VOLUME)
        return false;

    // Do a round of interval evaluation for tree pruning
    eval_i(tree, (Interval){r.X[0], r.X[r.ni]},
                 (Interval){r.Y[0], r.Y[r.nj]},
                 (Interval){r.Z[0], r.Z[r.nk]});
    disable_nodes(tree);

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

    return true;
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
            dummy.X[i] = v0[i][0] * (1 - p[i]) + v1[i][0] * p[i];
            dummy.Y[i] = v0[i][1] * (1 - p[i]) + v1[i][1] * p[i];
            dummy.Z[i] = v0[i][1] * (1 - p[i]) + v1[i][2] * p[i];
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
            push_vert(ex[count], ey[count], ez[count]);
            count++;
        }
        else if (c.cmd == InterpolateCommand::CACHED)
        {
            unsigned i = c.cached;
            push_vert(ex[i], ey[i], ez[i]);
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
            if ((v0 == c.v0 && v1 == c.v1) || (v0 == c.v1 && v1 == c.v0))
            {
                next.cmd = InterpolateCommand::CACHED;
                next.cached = count;
            }
            count++;
        }
    }

    queue.push_back(next);
    if (next.cmd == InterpolateCommand::INTERPOLATE && count + 1 == MIN_VOLUME)
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

Vec3f Mesher::plane_intersection(const Vec3f& pa, const Vec3f& na,
                                 const Vec3f& pb, const Vec3f& nb,
                                 const Vec3f& pc, const Vec3f& nc,
                                 const Vec3f& fallback)
{
    Eigen::Matrix3d n;
    n.col(0) << na;
    n.col(1) << nb;
    n.col(2) << nc;

    const double det = n.determinant();
    if (fabs(det) <= 0.01)
        return fallback;

    // Graphics Gems 1
    // Intersection of Three Planes
    // Ronald Goldman
    Eigen::Vector3d out =
        (pa.dot(na)*nb.cross(nc) +
         pb.dot(nb)*nc.cross(na) +
         pc.dot(nc)*na.cross(nb)) / det;

    return (Vec3f){static_cast<float>(out[0]),
                   static_cast<float>(out[1]),
                   static_cast<float>(out[2])};
}

Vec3f Mesher::edge_normal(const Vec3f& a, const Vec3f& b, const Vec3f& c)
{
    Eigen::Vector3d out = (b-a).cross(c-a).cross(b-a);

    return (Vec3f){static_cast<float>(out[0] / out.norm()),
                   static_cast<float>(out[1] / out.norm()),
                   static_cast<float>(out[2] / out.norm())};
}

void Mesher::triangulate_region(const Region& r)
{
    // If we can calculate all of the points in this region with a single
    // eval_r call, then do so.  This large chunk will be used in future
    // recursive calls to make things more efficient.
    bool loaded_data = !has_data;
    if (loaded_data)
        loaded_data = load_packed(r);

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

float* Mesher::get_verts(unsigned* count)
{
    // There are 9 floats in each triangle
    *count = triangles.size() * 9;

    float* v = (float*)malloc(sizeof(float) * (*count));

    unsigned i = 0;
    for (auto t : triangles)
        for (auto v : {t.a, t.b, t.c})
            for (int j=0; j < 3; ++j)
                v[i++] = v[j];

    return v;
}
