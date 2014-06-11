#ifndef SHAPE_H
#define SHAPE_H

#include <string>

#include "bounds.h"
#include "transform.h"

struct MathTree_;

struct MathShape
{
    MathShape(std::string math);
    MathShape(std::string math, float xmin, float ymin,
              float xmax, float ymax);
    MathShape(std::string math, float xmin, float ymin, float zmin,
              float xmax, float ymax, float zmax);
    MathShape(std::string math, Bounds bounds);

    struct MathTree_* getTree();

    MathShape map(Transform t) const;

    struct MathTree_* tree;
    std::string math;
    Bounds bounds;
};

#endif // SHAPE_H
