#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include <vector>
#include <list>
#include <map>
#include <array>

#include "util/region.h"
#include "util/vec3f.h"

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
    Mesher(struct MathTree_* tree, bool detect_edges);
    ~Mesher();

    void triangulate_region(const Region& r);

    // Actual vertices
    std::vector<float> verts;
protected:
    enum FeatureType { FEATURE_CORNER, FEATURE_EDGE_AB_C,
                       FEATURE_EDGE_BC_A, FEATURE_EDGE_CA_B };

    void get_normals(Vec3f* normals);

    /*
     * Returns the normal of the most recent triangle added to the mesh.
     */
    Vec3f get_triangle_normal(int index=-1) const;

    /*
     *  Processes a sharp feature in the most recent triangle.
     *  normals is an array of 3 normals at the triangle's corners.
     *
     *  Returns false if this was a corner feature with a point that lies
     *  outside of the triangle's bounds (and was thus not handled);
     *  otherwise returns true.
     */
    bool process_feature(FeatureType t, Vec3f* normals);

    void check_feature();
    void push_vert(const Vec3f& v);

    /*
     *  Attempts to evaluate every voxel in the given region.
     *  Returns false if there are too many voxels; true on success.
     */
    bool load_packed(const Region& r);

    void unload_packed();
    void get_corner_data(const Region& r, float d[8]);
    void eval_zero_crossings(Vec3f* v0, Vec3f* v1, unsigned count);
    void flush_queue();
    void end_voxel();
    void interpolate_between(const Vec3f& v0, const Vec3f& v1);
    void process_tet(const Region& r, const float* const d, const int tet);

    // Finds the intersection of three planes, each defined by
    // point-on-plane and normal vectors.
    // If the determinant is 0, return fallback.
    Vec3f plane_intersection(const Vec3f& xa_, const Vec3f& na_,
                             const Vec3f& xb_, const Vec3f& nb_,
                             const Vec3f& xc_, const Vec3f& nc_,
                             const Vec3f& fallback);

    // Given two planes (one containing point a with normal na, the other
    // containing point b with normal nb), find the point closest to c that
    // is on the edge representing the intersection of the planes.
    Vec3f plane_plane_point(const Vec3f& a, const Vec3f& na,
                            const Vec3f& b, const Vec3f& nb,
                            const Vec3f& c);

    // Returns the edge location for a triangle with A and C on the
    // same face (and B on a different face).
    // If the determinant is 0, return fallback.
    Vec3f edge_feature_point(const Vec3f& a, const Vec3f& na,
                             const Vec3f& b, const Vec3f& nb,
                             const Vec3f& c, const Vec3f& nc,
                             const Vec3f& fallback);

    // Checks whether point p_ is in the triangle abc
    // (when projected into the same plane)
    bool point_in_triangle(const Vec3f& a_, const Vec3f& b_, const Vec3f& c_,
                           const Vec3f& p_);

    // Finds the normal of a plane perpendicular to triangle abc
    // and containing edge ab.
    Vec3f edge_normal(const Vec3f& a, const Vec3f& b, const Vec3f& c);

    // Marks that the first edge of the most recent triangle can be swapped.
    void mark_swappable();

    // MathTree that we're evaluating
    struct MathTree_* tree;
    bool detect_edges;

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

    std::map<std::array<float, 6>, unsigned> swappable;
};

void triangulate(struct MathTree_* tree, Region r, bool detect_edges,
                 float** const verts, unsigned* const count);

#endif
