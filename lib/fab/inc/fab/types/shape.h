#ifndef SHAPE_H
#define SHAPE_H

#include <boost/python.hpp>

#include <string>
#include <memory>

#include "fab/types/bounds.h"
#include "fab/types/transform.h"

#include "fab/tree/tree.h"

/** Represents a math expression and a set of bounds. */
struct Shape
{
    /* Constructors all throw fab::ParseError if parsing fails. */
    explicit Shape();
    Shape(boost::python::object obj);
    Shape(std::string math);
    Shape(std::string math, float xmin, float ymin,
              float xmax, float ymax);
    Shape(std::string math, float xmin, float ymin, float zmin,
              float xmax, float ymax, float zmax);
    Shape(std::string math, Bounds bounds);

    /** Returns a new shape with re-mapped coordinates and bounds. */
    Shape map(Transform t) const;

    std::string math;
    Bounds bounds;

    std::shared_ptr<MathTree> tree;
    int r, g, b;
};

Shape operator~(const Shape& a);
Shape operator|(const Shape& a, const Shape& b);
Shape operator&(const Shape& a, const Shape& b);

#endif // SHAPE_H
