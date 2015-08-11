#ifndef SHAPE_H
#define SHAPE_H

#include <boost/python.hpp>

#include <string>
#include <memory>

#include "fab/types/bounds.h"
#include "fab/types/transform.h"

#include "fab/tree/tree.h"

typedef std::tuple<int,int,int> int3;

/** Represents a math expression and a set of bounds. */
struct Shape
{
    /* Constructor throws fab::ParseError if parsing fails. */
    Shape(std::string math="=1;", Bounds bounds=Bounds(),
          int3 color=int3(-1,-1,-1));

    /* Simpler constructor that can be wrapped by Boost */
    Shape(std::string math, Bounds bounds, int r, int g, int b);

    /*
     *  Raw constructor function
     */
    static boost::python::object init(boost::python::tuple args,
                                      boost::python::dict kwargs);

    /*
     *  Builder function, which calls one of the above constructors.
     */
    static Shape* builder(boost::python::tuple args,
                          boost::python::dict kwargs);

    std::string repr() const;

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
