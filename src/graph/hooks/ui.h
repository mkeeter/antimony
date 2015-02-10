#ifndef UI_HOOKS
#define UI_HOOKS

#include <boost/python.hpp>

class ViewportScene;

struct ScriptUIHooks
{
    ScriptUIHooks() : scene(NULL) {}
    static boost::python::object point(boost::python::tuple args,
                                       boost::python::dict kwargs);

    ViewportScene* scene;
};

#endif
