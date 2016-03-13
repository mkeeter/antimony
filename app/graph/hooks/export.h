#pragma once

#include <boost/python.hpp>

#include <QString>

#include "fab/types/bounds.h"
#include "fab/types/shape.h"

class Node;
class NodeProxy;

struct ScriptExportHooks
{
    ScriptExportHooks() : node(NULL), proxy(NULL), called(false) {}

    /*
     *  Creates an export task that saves an stl mesh
     */
    static boost::python::object stl(
            boost::python::tuple args,
            boost::python::dict kwargs);

    /*
     *  Creates an export task that saves a greyscale heightmap
     */
    static boost::python::object heightmap(
            boost::python::tuple args,
            boost::python::dict kwargs);

    /*
     *  Helper function that retrieves an object of the given type
     *  from the keyword arguments dictionary
     */
    template <class T>
    static T get_object(std::string name, boost::python::dict kwargs, T d);

    /*
     *  Checks to see if the first argument is a shape;
     *  otherwise, throws an AppHooks::Exception
     */
    static Shape get_shape(boost::python::tuple args);

    /*
     *  Pads the bounds by |xmax - xmin|/20
     *  (and similar for other axes)
     */
    static Bounds pad_bounds(Bounds b);

    Node* node;
    NodeProxy* proxy;
    bool called;
};
