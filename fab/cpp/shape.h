#ifndef SHAPE_H
#define SHAPE_H

#include <string>
#include <memory>

#include "cpp/bounds.h"
#include "cpp/transform.h"

#include "tree/tree.h"

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

    /** Returns a new shape with re-mapped coordinates and bounds. */
    MathShape map(Transform t) const;

    const std::string math;
    const Bounds bounds;

protected:
    std::shared_ptr<MathTree> tree;
};

MathShape operator~(const MathShape& a);
MathShape operator|(const MathShape& a, const MathShape& b);
MathShape operator&(const MathShape& a, const MathShape& b);

#endif // SHAPE_H
