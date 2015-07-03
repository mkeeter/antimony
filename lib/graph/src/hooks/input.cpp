#include <Python.h>

#include "graph/hooks/input.h"
#include "graph/hooks/hooks.h"
#include "graph/node.h"

void InputHook::call(std::string name, boost::python::object type)
{
    // Call the type constructor with no arguments
    call_with_default(name, type, type());
}

void InputHook::call_with_default(
        std::string name, boost::python::object type,
        boost::python::object default_value)
{
    if (!PyType_Check(type.ptr()))
        throw Hooks::Exception("Invalid second argument (must be a type)");

    auto repr = PyObject_Repr(default_value.ptr());
    if (PyErr_Occurred())
    {
        PyErr_Clear();
        throw Hooks::Exception("Failed to get __repr__ of argument");
    }

    node->makeDatum(name, (PyTypeObject*)type.ptr(),
                    std::string(PyUnicode_AsUTF8(repr)));
    Py_DECREF(repr);
}
