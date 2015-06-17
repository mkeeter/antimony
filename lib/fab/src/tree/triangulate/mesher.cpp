#include <iostream>
#include <set>

#include "tree/triangulate/mesher.h"

#include "tree/tree.h"
#include "tree/eval.h"
#include "util/switches.h"

#if MIN_VOLUME < 60
#error "MIN_VOLUME is below minimum for meshing implementation."
#endif

static const uint8_t VERTEX_LOOP[] = {6, 4, 5, 1, 3, 2, 6};

// Based on which vertices are filled, this map tells you which
// edges to interpolate between when forming zero, one, or two
// triangles for a tetrahedron.
// (filled vertex is first in the pair, and is given as a tetrahedron vertex
//  so you have to translate into a proper cube vertex).
static const int EDGE_MAP[16][2][3][2] = {
    {{{-1,-1}, {-1,-1}, {-1,-1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // ----
    {{{ 0, 2}, { 0, 1}, { 0, 3}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // ---0
    {{{ 1, 2}, { 1, 3}, { 1, 0}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // --1-
    {{{ 1, 2}, { 1, 3}, { 0, 3}}, {{ 0, 3}, { 0, 2}, { 1, 2}}}, // --10
    {{{ 2, 0}, { 2, 3}, { 2, 1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // -2--
    {{{ 0, 3}, { 2, 3}, { 2, 1}}, {{ 2, 1}, { 0, 1}, { 0, 3}}}, // -2-0
    {{{ 2, 0}, { 2, 3}, { 1, 0}}, {{ 2, 3}, { 1, 3}, { 1, 0}}}, // -21-
    {{{ 2, 3}, { 1, 3}, { 0, 3}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // -210

    {{{ 3, 0}, { 3, 1}, { 3, 2}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 3---
    {{{ 3, 2}, { 0, 2}, { 0, 1}}, {{ 3, 1}, { 3, 2}, { 0, 1}}}, // 3--0
    {{{ 1, 2}, { 3, 2}, { 3, 0}}, {{ 3, 0}, { 1, 0}, { 1, 2}}}, // 3-1-
    {{{ 1, 2}, { 3, 2}, { 0, 2}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 3-10
    {{{ 3, 0}, { 3, 1}, { 2, 1}}, {{ 2, 1}, { 2, 0}, { 3, 0}}}, // 32--
    {{{ 3, 1}, { 2, 1}, { 0, 1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 32-0
    {{{ 3, 0}, { 1, 0}, { 2, 0}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 321-
    {{{-1,-1}, {-1,-1}, {-1,-1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 3210
};

Mesher::Mesher(MathTree* tree, bool detect_edges, volatile int* halt)
    : tree(tree), detect_edges(detect_edges), halt(halt),
      data(new float[MIN_VOLUME]), has_data(false),
      X(new float[MIN_VOLUME]),
      Y(new float[MIN_VOLUME]),
      Z(new float[MIN_VOLUME]),
      ex(new float[MIN_VOLUME]),
      ey(new float[MIN_VOLUME]),
      ez(new float[MIN_VOLUME]),
      nx(new float[MIN_VOLUME]),
      ny(new float[MIN_VOLUME]),
      nz(new float[MIN_VOLUME]),
      voxel_start(triangles.end())
{
    // Nothing to do here
}

Mesher::~Mesher()
{
    for (auto ptr : {data, X, Y, Z, ex, ey, ez, nx, ny, nz})
        delete [] ptr;
}


// Estimate the normals of a set of points.
std::list<Vec3f> Mesher::get_normals(const std::list<Vec3f>& points)
{
    // Find epsilon as the single shortest side length divided by 10.
    float epsilon = INFINITY;
    auto j = points.begin();
    j++;
    for (auto i = points.begin(); j != points.end(); ++i, ++j)
    {
        if (j != points.end())
        {
            auto d = *j - *i;
            epsilon = fmin(epsilon, d.norm() / 100.0f);
        }
    }

    // We'll be evaluating a dummy region to numerically estimate gradients
    Region dummy;
    dummy.voxels = points.size() * 7;
    if (dummy.voxels >= MIN_VOLUME)
        std::cerr << "Error: too many normals to calculate at once!"
                  << std::endl;
    dummy.X = nx;
    dummy.Y = ny;
    dummy.Z = nz;

    // Load position data into the dummy region
    int i=0;
    for (auto v : points)
    {
        dummy.X[i]   = v[0];
        dummy.X[i+1] = v[0] + epsilon;
        dummy.X[i+2] = v[0];
        dummy.X[i+3] = v[0];
        dummy.X[i+4] = v[0] - epsilon;
        dummy.X[i+5] = v[0];
        dummy.X[i+6] = v[0];

        dummy.Y[i]   = v[1];
        dummy.Y[i+1] = v[1];
        dummy.Y[i+2] = v[1] + epsilon;
        dummy.Y[i+3] = v[1];
        dummy.Y[i+4] = v[1];
        dummy.Y[i+5] = v[1] - epsilon;
        dummy.Y[i+6] = v[1];

        dummy.Z[i]   = v[2];
        dummy.Z[i+1] = v[2];
        dummy.Z[i+2] = v[2];
        dummy.Z[i+3] = v[2] + epsilon;
        dummy.Z[i+4] = v[2];
        dummy.Z[i+5] = v[2];
        dummy.Z[i+6] = v[2] - epsilon;
        i += 7;
    }

    float* out = eval_r(tree, dummy);

    // Extract normals from the evaluated data.
    std::list<Vec3f> normals;
    i = 0;
    for (auto v : points)
    {
        const float dx = (out[i+1] - out[i]) - (out[i+4] - out[i]);
        const float dy = (out[i+2] - out[i]) - (out[i+5] - out[i]);
        const float dz = (out[i+3] - out[i]) - (out[i+6] - out[i]);
        normals.push_back(Vec3f(dx, dy, dz).normalized());
        i += 7;
    }

    return normals;
}

// Mark that the first edge of the most recent triangle is swappable
// (as part of feature detection / extraction).
void Mesher::push_swappable_triangle(Triangle t)
{
    auto found = swappable.find(t.ab_());
    if (found != swappable.end())
    {
        found->second->b = t.c;
        t.b = found->second->c;
        triangles.push_back(t);
        swappable.erase(found);
    }
    else
    {
        triangles.push_back(t);

        // Store an iterator pointing to the new triangle.
        auto itr = triangles.end();
        itr--;
        swappable[t.ba_()] = itr;
    }

    // Adjust voxel_end so that it points to the first new triangle.
    if (voxel_end == triangles.end())
        voxel_end--;
}

std::list<Vec3f> Mesher::get_contour()
{
    // Find all of the singular edges in this fan
    // (edges that aren't shared between multiple triangles).
    std::set<std::array<float, 6>> valid_edges;
    for (auto itr=voxel_start; itr != voxel_end; ++itr)
    {
        if (valid_edges.count(itr->ba_()))
            valid_edges.erase(itr->ba_());
        else
            valid_edges.insert(itr->ab_());

        if (valid_edges.count(itr->cb_()))
            valid_edges.erase(itr->cb_());
        else
            valid_edges.insert(itr->bc_());

        if (valid_edges.count(itr->ac_()))
            valid_edges.erase(itr->ac_());
        else
            valid_edges.insert(itr->ca_());
    }

    std::set<std::array<float, 3>> in_fan;

    std::list<Vec3f> contour = {voxel_start->a};
    in_fan.insert(voxel_start->a_());
    in_fan.insert(voxel_start->b_());
    in_fan.insert(voxel_start->c_());

    fan_start = voxel_start;
    voxel_start++;

    while (contour.size() == 1 || contour.front() != contour.back())
    {
        std::list<Triangle>::iterator itr;
        for (itr=fan_start; itr != voxel_end; ++itr)
        {
            const auto& t = *itr;
            if (contour.back() == t.a && valid_edges.count(t.ab_()))
            {
                contour.push_back(t.b);
                break;
            }

            if (contour.back() == t.b && valid_edges.count(t.bc_()))
            {
                contour.push_back(t.c);
                break;
            }

            if (contour.back() == t.c && valid_edges.count(t.ca_()))
            {
                contour.push_back(t.a);
                break;
            }
        }
        // If we broke out of the loop (meaning itr is pointing to a relevant
        // triangle which should be moved forward to before voxel_start), then
        // push the list around and update iterators appropriately.
        if (itr != voxel_end)
        {
            in_fan.insert(itr->a_());
            in_fan.insert(itr->b_());
            in_fan.insert(itr->c_());

            if (itr == voxel_start)
            {
                voxel_start++;
            }
            else if (itr != fan_start)
            {
                const Triangle t = *itr;
                triangles.insert(voxel_start, t);
                itr = triangles.erase(itr);
                itr--;
            }
        }
    }

    // Special case to catch triangles that are part of a particular fan but
    // don't have any edges in the contour (which can happen!).
    for (auto itr=voxel_start;  itr != voxel_end; ++itr)
    {
        if (in_fan.count(itr->a_()) &&
            in_fan.count(itr->b_()) &&
            in_fan.count(itr->c_()))
        {
            if (itr == voxel_start)
            {
                voxel_start++;
            }
            else if (itr != fan_start)
            {
                const Triangle t = *itr;
                triangles.insert(voxel_start, t);
                itr = triangles.erase(itr);
                itr--;
            }
        }
    }

    // Remove the last point of the contour, since it's a closed loop.
    contour.pop_back();
    return contour;
}

void Mesher::check_feature()
{
    auto contour = get_contour();
    const auto normals = get_normals(contour);

    // Find the largest cone and the normals that enclose
    // the largest angle as n0, n1.
    float theta = 1;
    Vec3f n0, n1;
    for (auto ni : normals)
    {
        for (auto nj : normals)
        {
            float dot = ni.dot(nj);
            if (dot < theta)
            {
                theta = dot;
                n0 = ni;
                n1 = nj;
            }
        }
    }

    // If there isn't a feature in this fan, then return immediately.
    if (theta > 0.9)
        return;

    // Decide whether this is a corner or edge feature.
    const Vec3f nstar = n0.cross(n1);
    float phi = 0;
    for (auto n : normals)
        phi = fmax(phi, fabs(nstar.dot(n)));
    bool edge = phi < 0.7;

    // Find the center of the contour.
    Vec3f center(0, 0, 0);
    for (auto c : contour)
        center += c;
    center /= contour.size();

    // Construct the matrices for use in our least-square fit.
    Eigen::MatrixX3d A(normals.size(), 3);
    {
        int i=0;
        for (auto n : normals)
            A.row(i++) << n.transpose();
    }

    // When building the second matrix, shift position values to be centered
    // about the origin (because that's what the least-squares fit will
    // minimize).
    Eigen::VectorXd B(normals.size(), 1);
    {
        auto n = normals.begin();
        auto c = contour.begin();
        int i=0;
        while (n != normals.end())
            B.row(i++) << (n++)->dot(*(c++) - center);
    }

    // Use singular value decomposition to solve the least-squares fit.
    Eigen::JacobiSVD<Eigen::MatrixX3d> svd(A, Eigen::ComputeFullU |
                                              Eigen::ComputeFullV);

    // Set the smallest singular value to zero to make fitting happier.
    if (edge)
    {
        auto singular = svd.singularValues();
        svd.setThreshold(singular.minCoeff() / singular.maxCoeff() * 1.01);
    }

    // Solve for the new point's position.
    const Vec3f new_pt = svd.solve(B) + center;

    // Erase this triangle fan, as we'll be inserting a vertex in the center.
    triangles.erase(fan_start, voxel_start);

    // Construct a new triangle fan.
    contour.push_back(contour.front());
    {
        auto p0 = contour.begin();
        auto p1 = contour.begin();
        p1++;
        while (p1 != contour.end())
            push_swappable_triangle(Triangle(*(p0++), *(p1++), new_pt));
    }
}

void Mesher::remove_dupes()
{
    std::map<std::array<float, 3>, size_t> verts;
    std::set<std::array<size_t, 3>> tris;
    size_t vertex_id = 0;

    for (auto itr=triangles.begin(); itr != triangles.end(); ++itr)
    {
        std::array<size_t, 3> t;
        int i=0;
        // For each vertex, find whether it's already in our vertex
        // set.  Create an array t with vertex indicies.
        for (auto v : {itr->a_(), itr->b_(), itr->c_()})
        {
            auto k = verts.find(v);
            if (k != verts.end())
            {
                t[i++] = k->second;
            }
            else
            {
                verts[v] = vertex_id;
                t[i++] = vertex_id;
                vertex_id++;
            }
        }

        // Check to see if there are any other triangles that use these
        // three vertices; if so, delete this triangle.
        std::sort(t.begin(), t.end());
        if (tris.count(t))
        {
            itr = triangles.erase(itr);
            itr--;
        }
        else
        {
            tris.insert(t);
        }
    }
}

void Mesher::prune_flags()
{
    std::set<std::array<float, 6>> edges;
    for (auto t : triangles)
    {
        edges.insert(t.ab_());
        edges.insert(t.bc_());
        edges.insert(t.ca_());
    }

    for (auto itr=triangles.begin(); itr != triangles.end(); ++itr)
    {
        if (!edges.count(itr->ba_()) ||
            !edges.count(itr->cb_()) ||
            !edges.count(itr->ac_()))
        {
            itr = triangles.erase(itr);
            itr--;
        }
    }
}

// Loads a vertex into the vertex list.
// If this vertex completes a triangle, check for features.
void Mesher::push_vert(const float x, const float y, const float z)
{
    triangle.push_back(Vec3f(x, y, z));
    if (triangle.size() == 3)
    {
        triangles.push_back(Triangle(triangle[0], triangle[1], triangle[2]));
        triangle.clear();

        // If this is the first triangle being constructed (or voxel_start has
        // just been cleared), store an iterator to this triangle so that we
        // know where the next triangle fan begins.
        if (voxel_start == triangles.end())
            voxel_start--;
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

    // We've already run interval evaluation for this region
    // (at the beginning of triangulate_region), so here we'll
    // just disable inactive nodes.
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
    packed.imin = r.imin;
    packed.jmin = r.jmin;
    packed.kmin = r.kmin;
    packed.ni = r.ni;
    packed.nj = r.nj;
    packed.nk = r.nk;
    packed.X = X;
    packed.Y = Y;
    packed.Z = Z;
    packed.voxels = voxels;

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

bool Mesher::get_corner_data(const Region& r, float d[8])
{
    bool has_positive = false;
    bool has_negative = false;
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

        has_negative |= d[i] < 0;
        has_positive |= d[i] >= 0;
    }

    return has_positive && has_negative;
}

void Mesher::eval_zero_crossings(Vec3f* v0, Vec3f* v1, unsigned count)
{
    float p[count];
    for (unsigned i=0; i < count; ++i)
        p[i] = 0.5;

    float step = 0.25;

    Region dummy;
    dummy.X = ex;
    dummy.Y = ey;
    dummy.Z = ez;
    dummy.voxels = count;

    for (int iteration=0; iteration < 8; ++iteration)
    {
        // Load new data into the x, y, z arrays.
        for (unsigned i=0; i < count; i++)
        {
            dummy.X[i] = v0[i][0] * (1 - p[i]) + v1[i][0] * p[i];
            dummy.Y[i] = v0[i][1] * (1 - p[i]) + v1[i][1] * p[i];
            dummy.Z[i] = v0[i][2] * (1 - p[i]) + v1[i][2] * p[i];
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
        else if (c.cmd == InterpolateCommand::END_OF_VOXEL)
        {
            if (detect_edges)
            {
                // Clear voxel_end
                // (it will be reset when the next triangle is pushed)
                voxel_end = triangles.end();

                // Then, iterate until no more features are found in
                // the current voxel.
                while (voxel_start != voxel_end &&
                       voxel_start != triangles.end())
                {
                    check_feature();
                }

                // Clear voxel_start
                // (it will be reset when the next triangle is pushed)
                voxel_start = triangles.end();
            }
        }
    }
    queue.clear();
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


void Mesher::triangulate_tet(const Region& r, const float* const d,
                             const int t)
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

void Mesher::triangulate_voxel(const Region& r, const float* const d)
{
    for (int t=0; t < 6; ++t)
        triangulate_tet(r, d, t);
}

void Mesher::triangulate_region(const Region& r)
{
    // Early abort if the halt flag is set
    if (*halt)
        return;

    // Do a round of interval evaluation to skip empty regions.
    auto interval = eval_i(tree, (Interval){r.X[0], r.X[r.ni]},
                                 (Interval){r.Y[0], r.Y[r.nj]},
                                 (Interval){r.Z[0], r.Z[r.nk]});
    if (interval.lower > 0 || interval.upper < 0)
        return;

    // If we can calculate all of the points in this region with a single
    // eval_r call, then do so.  This large chunk will be used in future
    // recursive calls to make things more efficient.
    bool loaded_data;
    if (!has_data)
        loaded_data = load_packed(r);
    else
        loaded_data = false;

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

        if (get_corner_data(r, d))
        {
            // Triangulate this particular voxel
            triangulate_voxel(r, d);

            // Mark that a voxel has ended
            // (which triggers mesh refinment)
            queue.push_back((InterpolateCommand){
                    .cmd=InterpolateCommand::END_OF_VOXEL});
        }
    }

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
    if (detect_edges)
    {
        remove_dupes();
        prune_flags();
    }

    // There are 9 floats in each triangle
    *count = triangles.size() * 9;

    float* out = (float*)malloc(sizeof(float) * (*count));

    unsigned i = 0;
    for (auto t : triangles)
        for (auto v : {t.a, t.b, t.c})
            for (int j=0; j < 3; ++j)
                out[i++] = v[j];

    return out;
}
