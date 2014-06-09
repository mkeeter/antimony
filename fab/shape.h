#ifndef SHAPE_H
#define SHAPE_H

#include <string>

struct MathTree_;

struct MathShape
{
    MathShape(std::string math, float xmin, float ymin, float zmin,
              float xmax, float ymax, float zmax);
    struct MathTree_* getTree();

    struct MathTree_* tree;
    std::string math;
    float xmin, ymin, zmin;
    float xmax, ymax, zmax;
};

#endif // SHAPE_H
