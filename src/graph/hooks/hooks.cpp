#include <boost/python.hpp>

#include "graph/hooks/hooks.h"
#include "graph/hooks/input.h"
#include "graph/hooks/output.h"

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

    register_exception_translator<hooks::HookException>(
            hooks::onHookException);
}

void hooks::preInit()
{
    PyImport_AppendInittab("_hooks", PyInit__hooks);
}
