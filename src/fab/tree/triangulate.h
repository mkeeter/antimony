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
    Mesher(struct MathTree_* tree);
    ~Mesher();

    void triangulate_region(const Region& r);

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

    // Marks that the first edge of the most recent triangle can be swapped.
    void mark_swappable();

    // MathTree that we're evaluating
    struct MathTree_* tree;

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

void triangulate(struct MathTree_* tree, Region r,
                 float** const verts, unsigned* const count);

#endif
