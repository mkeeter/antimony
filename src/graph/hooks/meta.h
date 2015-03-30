#ifndef META_HOOKS
#define META_HOOKS

#include <boost/python.hpp>

#include <QString>

#include "fab/types/bounds.h"
#include "fab/types/shape.h"

class Node;
class InspectorExportButton;

struct ScriptMetaHooks
{
    ScriptMetaHooks() : button(NULL) {}

    static boost::python::object export_stl(
            boost::python::tuple args,
            boost::python::dict kwargs);
    static boost::python::object export_heightmap(
            boost::python::tuple args,
            boost::python::dict kwargs);

    static Shape get_shape(boost::python::tuple args);
    static Bounds get_bounds(boost::python::dict kwargs);
    static float get_resolution(boost::python::dict kwargs);
    static bool get_pad(boost::python::dict kwargs);
    static QString get_filename(boost::python::dict kwargs);

    static Bounds pad_bounds(Bounds b);

    InspectorExportButton* button;
};

#endif
