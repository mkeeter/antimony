#ifndef MESHER_H
#define MESHER_H

#include <list>
#include <map>
#include <array>
#include <vector>

#include "tree/triangulate/triangle.h"

#include "util/region.h"

// Forward declaration of MathTree
struct MathTree_;

struct InterpolateCommand {
    enum {INTERPOLATE, CACHED, END_OF_VOXEL} cmd;
    Vec3f v0;
    Vec3f v1;
    unsigned cached;
};

class Mesher {
public:
    Mesher(struct MathTree_* tree, bool detect_edges, volatile int* halt);
    ~Mesher();

    /*
     *  Recursively triangulates the given voxel region.
     */
    void triangulate_region(const Region& r);

    /*
     *  Allocates memory (using malloc) and returns a flat set of vertices.
     *  count is set to the number of floats allocated
     *  (i.e. number of vertices * 3)
     */
    float* get_verts(unsigned* count);

protected:
    /*
     *  Finds the normals of each vertex on the triangle.
     *  Returns a Triangle with the corners as the new normals.
     */
     std::list<Vec3f> get_normals(const std::list<Vec3f>& t);

    /*
     *  Records another vertex.
     *  Calls add_triangle if this vertex completes a triangle.
     */
    void push_vert(const float x, const float y, const float z);

    /*
     *  Attempts to evaluate every voxel in the given region.
     *  Returns false if there are too many voxels; true on success.
     */
    bool load_packed(const Region& r);
    void unload_packed();

    /*
     *  Looks up the corner values for the given region, storing them in d.
     *  Returns true if this voxel has anything of interest in it.
     */
    bool get_corner_data(const Region& r, float d[8]);

    /*
     *  Performs binary search on a set of edges.
     *  v0 and v1 must contain 'count' points.
     *  Found x, y, z value are stored in ex, ey, ez.
     */
    void eval_zero_crossings(Vec3f* v0, Vec3f* v1, unsigned count);

    /*
     *  Flushes the command queue.
     *  This will involve calculating a set of interpolated positions
     *  then assembling them into triangles.
     */
    void flush_queue();

    /*
     *  Schedules an interpolation command in the command queue.
     */
    void interpolate_between(const Vec3f& v0, const Vec3f& v1);

    /*
     *  Evaluates the given voxel.
     *      r is the voxel region
     *      d is the corner values
     */
    void triangulate_voxel(const Region& r, const float* const d);

    /*
     *  Evaluates the given tetrahedron in a voxel.
     *      r is the voxel region
     *      d is the corner values
     *      t is the tetrahedron's ID.
     */
    void triangulate_tet(const Region& r, const float* const d, const int t);

    /*
     *  Marks that the first edge of this triangle is swappable,
     *  and performs the swap if a match is found.
     */
    void push_swappable_triangle(Triangle t);

    /*
     *  Check the most recent fan (from voxel_start to triangles.end())
     *  for features and process them if they are found.
     */
    void check_feature();

    /*
     *  Removes duplicates from the triangle list.
     */
    void remove_dupes();

    /*
     *  Removes triangles with edges that aren't connected to the
     *  rest of the mesh (which happens sometimes when refining geometry).
     */
    void prune_flags();

    /*
     *  Returns a closed contour that traces the most recent fan.
     *
     *  Modifies triangles, voxel_start, and fan_start so that the
     *  most recent fan is stored between fan_start and voxel_start.
     */
    std::list<Vec3f> get_contour();

    // MathTree that we're evaluating
    struct MathTree_* tree;
    bool detect_edges;
    volatile int* halt;

    // Cached region and data from an eval_r call
    Region packed;
    float* data;
    bool has_data;

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
    std::list<InterpolateCommand> queue;

    // Triangle that's being constructed
    std::vector<Vec3f> triangle;

    // List of existing triangles
    std::list<Triangle> triangles;

    std::list<Triangle>::iterator voxel_start;
    std::list<Triangle>::iterator voxel_end;
    std::list<Triangle>::iterator fan_start;
    std::map<std::array<float, 6>, std::list<Triangle>::iterator> swappable;
};

#endif
