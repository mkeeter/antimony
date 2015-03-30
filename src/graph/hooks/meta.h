#ifndef META_HOOKS
#define META_HOOKS

#include <boost/python.hpp>

class Node;
class InspectorExportButton;

struct ScriptMetaHooks
{
    ScriptMetaHooks() : node(NULL), button(NULL) {}

    static boost::python::object export_stl(
            boost::python::tuple args,
            boost::python::dict kwargs);

    Node* node;
    InspectorExportButton* button;
};

#endif
