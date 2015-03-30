#ifndef META_HOOKS
#define META_HOOKS

#include <boost/python.hpp>

class Node;
class InspectorExportButton;

struct ScriptMetaHooks
{
    ScriptMetaHooks() : button(NULL) {}

    static boost::python::object export_stl(
            boost::python::tuple args,
            boost::python::dict kwargs);

    InspectorExportButton* button;
};

#endif
