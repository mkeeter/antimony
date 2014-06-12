#ifndef SHAPE_H
#define SHAPE_H

#include <string>

#include "bounds.h"
#include "transform.h"

struct MathTree_;

/** Represents a math expression and a set of bounds. */
struct MathShape
{
    /* Constructors all throw fab::ParseError if parsing fails. */
    MathShape(std::string math);
    MathShape(std::string math, float xmin, float ymin,
              float xmax, float ymax);
    MathShape(std::string math, float xmin, float ymin, float zmin,
              float xmax, float ymax, float zmax);
    MathShape(std::string math, Bounds bounds);

    ~MathShape();

    /** Returns the parsed MathTree object. */
    struct MathTree_* getTree();

    /** Returns a new shape with re-mapped coordinates and bounds. */
    MathShape map(Transform t) const;

    const std::string math;
    const Bounds bounds;
protected:
    struct MathTree_* tree;
};

#endif // SHAPE_H
