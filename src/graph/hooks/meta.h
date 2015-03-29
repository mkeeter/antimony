#ifndef META_HOOKS
#define META_HOOKS

#include <boost/python.hpp>

class Node;

struct ScriptMetaHooks
{
    ScriptMetaHooks() : node(NULL) {}

    static boost::python::object export_stl(
            boost::python::tuple args,
            boost::python::dict kwargs);

    Node* node;
};

#endif
