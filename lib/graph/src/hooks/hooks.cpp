#include <boost/python.hpp>

#include "graph/hooks/hooks.h"
#include "graph/hooks/input.h"

using namespace boost::python;

void Hooks::onException(const Hooks::Exception& e)
{
    PyErr_SetString(PyExc_RuntimeError, e.message.c_str());
}

BOOST_PYTHON_MODULE(_scriptIO)
{
    class_<InputHook>("InputHook", init<>())
        .def("__call__", &InputHook::call)
        .def("__call__", &InputHook::call_with_default);

    //class_<OutputHook>("OutputHook", init<>())
    //    .def("__call__", &OutputHook::call);

    register_exception_translator<Hooks::Exception>(
            Hooks::onException);
}

void Hooks::preInit()
{
    PyImport_AppendInittab("_scriptIO", PyInit__scriptIO);;
}


// Lazy initialization of the scriptIO module.
static PyObject* scriptIO_module = NULL;

void Hooks::load(PyObject* g, Node* n)
{
    if (scriptIO_module == NULL)
        scriptIO_module = PyImport_ImportModule("_scriptIO");

    auto input_func = PyObject_CallMethod(
            scriptIO_module, "InputHook", NULL);

    extract<InputHook&>(input_func)().node = n;

    PyDict_SetItemString(g, "input", input_func);
}
