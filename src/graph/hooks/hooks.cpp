#include <boost/python.hpp>
#include <boost/python/raw_function.hpp>

#include "graph/hooks/hooks.h"
#include "graph/hooks/input.h"
#include "graph/hooks/output.h"
#include "graph/hooks/title.h"
#include "graph/hooks/ui.h"

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
        .def("point", raw_function(&ScriptUIHooks::point, 3));

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

    extract<ScriptInputHook&>(input_func)().datum = d;
    extract<ScriptOutputHook&>(output_func)().datum = d;
    extract<ScriptTitleHook&>(title_func)().datum = d;

    PyDict_SetItemString(g, "input", input_func);
    PyDict_SetItemString(g, "output", output_func);
    PyDict_SetItemString(g, "title", title_func);
}
