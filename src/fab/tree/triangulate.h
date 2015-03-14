#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include <vector>
#include <list>

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

struct SwappableEdge {
    Vec3f v0;
    Vec3f v1;
    unsigned index;
};

class Mesher {
public:
    Mesher(struct MathTree_* tree);
    ~Mesher();

    // Actual vertices
    std::vector<float> verts;
protected:
    void get_normals(Vec3f* normals);
    void process_feature();
    void check_feature();
    void push_vert(const Vec3f& v);
    void load_packed(const Region& r);
    void unload_packed();
    void get_corner_data(const Region& r, float d[8]);
    void eval_zero_crossings(Vec3f* v0, Vec3f* v1, unsigned count);
    void flush_queue();
    void end_voxel();
    void interpolate_between(const Vec3f& v0, const Vec3f& v1);
    void process_tet(const Region& r, const float* const d, const int tet);
    void triangulate_region(const Region& r);

    // Cached region and data from an eval_r call
    Region packed;
    float* data;
    bool has_data;

    // MathTree that we're evaluating
    struct MathTree_* tree;

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

    // List of swappable edges
    std::list<SwappableEdge> swappable;
};

void triangulate(struct MathTree_* tree, Region r,
                 float** const verts, unsigned* const count);

#endif
