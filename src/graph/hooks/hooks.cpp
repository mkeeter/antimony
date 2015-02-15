#include <boost/python.hpp>
#include <boost/python/raw_function.hpp>

#include "graph/hooks/hooks.h"
#include "graph/hooks/input.h"
#include "graph/hooks/output.h"
#include "graph/hooks/title.h"
#include "graph/hooks/ui.h"

#include "graph/datum/datums/script_datum.h"
#include "graph/node/node.h"

#include "app/app.h"

using namespace boost::python;

void hooks::onHookException(const hooks::HookException& e)
{
    PyErr_SetString(PyExc_RuntimeError, e.message.c_str());
}

BOOST_PYTHON_MODULE(_hooks)
{
    class_<ScriptInputHook>("ScriptInputHook", init<>())
        .def("__call__", &ScriptInputHook::call)
        .def("__call__", &ScriptInputHook::call_with_default);

    class_<ScriptOutputHook>("ScriptOutputHook", init<>())
        .def("__call__", &ScriptOutputHook::call);

    class_<ScriptTitleHook>("ScriptTitleHook", init<>())
        .def("__call__", &ScriptTitleHook::call);

    class_<ScriptUIHooks>("ScriptUIHooks", init<>())
        .def("point", raw_function(&ScriptUIHooks::point),
                "point(x, y, z, r=5, color=(150, 150, 255), drag=None)\n"
                "    Constructs a point in the 3D viewport\n"
                "    x, y, z are the point's position\n\n"
                "    Valid kwargs:\n"
                "    r sets the point's radius\n"
                "    color sets the color as a 3-tuple (0-255)\n"
                "    drag is an optional callback:\n"
                "      It is invoked as drag(this, x, y, z)\n"
                "      where this is a representation of the parent\n"
                "      object and x, y, z are the position to which\n"
                "      the point has been dragged."
                )
        .def("wireframe", raw_function(&ScriptUIHooks::wireframe),
                "wireframe(pts, t=3, color=(150, 150, 255), close=false)\n"
                "    Constructs a wireframe in the 3D viewport\n"
                "    pts is a list of 3-float tuples\n\n"
                "    Valid kwargs:\n"
                "    t sets the line's thickness\n"
                "    color sets the color as a 3-tuple (0-255)\n"
                "    close makes the loop closed"
                );

    register_exception_translator<hooks::HookException>(
            hooks::onHookException);
}

// Lazy initialization of the hooks module.
static PyObject* _hooks_module = NULL;

void hooks::preInit()
{
    PyImport_AppendInittab("_hooks", PyInit__hooks);
}

void hooks::loadHooks(PyObject* g, ScriptDatum* d)
{
    if (_hooks_module == NULL)
        _hooks_module = PyImport_ImportModule("_hooks");

    auto input_func = PyObject_CallMethod(
            _hooks_module, "ScriptInputHook", NULL);
    auto output_func = PyObject_CallMethod(
            _hooks_module, "ScriptOutputHook", NULL);
    auto title_func = PyObject_CallMethod(
            _hooks_module, "ScriptTitleHook", NULL);
    auto ui_obj = PyObject_CallMethod(
            _hooks_module, "ScriptUIHooks", NULL);

    extract<ScriptInputHook&>(input_func)().datum = d;
    extract<ScriptOutputHook&>(output_func)().datum = d;
    extract<ScriptTitleHook&>(title_func)().datum = d;

    extract<ScriptUIHooks&>(ui_obj)().scene = App::instance()->getViewScene();
    extract<ScriptUIHooks&>(ui_obj)().node = static_cast<Node*>(d->parent());

    PyDict_SetItemString(g, "input", input_func);
    PyDict_SetItemString(g, "output", output_func);
    PyDict_SetItemString(g, "title", title_func);

    auto fab = PyImport_ImportModule("fab");
    PyObject_SetAttrString(fab, "ui", ui_obj);
    Py_DECREF(fab);
}
