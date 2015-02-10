#ifndef UI_HOOKS
#define UI_HOOKS

#include <boost/python.hpp>

class ViewportScene;
class Node;

struct ScriptUIHooks
{
    ScriptUIHooks() : scene(NULL) {}
    static boost::python::object point(boost::python::tuple args,
                                       boost::python::dict kwargs);

    Node* node;
    ViewportScene* scene;
};

#endif
