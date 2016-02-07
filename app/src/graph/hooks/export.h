#ifndef META_HOOKS
#define META_HOOKS

#include <boost/python.hpp>

#include <QString>

#include "fab/types/bounds.h"
#include "fab/types/shape.h"

class Node;
class InspectorExportButton;
class GraphScene;

struct ScriptExportHooks
{
    ScriptExportHooks() : node(NULL), scene(NULL), called(false) {}

    static boost::python::object stl(
            boost::python::tuple args,
            boost::python::dict kwargs);
    static boost::python::object heightmap(
            boost::python::tuple args,
            boost::python::dict kwargs);
            
    static boost::python::object voxels(
            boost::python::tuple args,
            boost::python::dict kwargs);

    static Shape get_shape(boost::python::tuple args);
    static Bounds get_bounds(boost::python::dict kwargs);
    static float get_resolution(boost::python::dict kwargs);
    static bool get_pad(boost::python::dict kwargs);
    static QString get_filename(boost::python::dict kwargs);

    static Bounds pad_bounds(Bounds b);

    Node* node;
    GraphScene* scene;
    bool called;
};

#endif
