#include <Python.h>

#include "graph/hooks/output.h"
#include "graph/hooks/hooks.h"
#include "graph/node.h"

void OutputHook::call(std::string name, boost::python::object obj)
{
    Hooks::checkName(name);

    auto repr = PyObject_Repr(obj.ptr());
    if (PyErr_Occurred())
    {
        PyErr_Clear();
        throw Hooks::Exception("Failed to get __repr__ of argument");
    }

    const bool result = node->makeDatum(
            name, obj.ptr()->ob_type,
            Datum::SIGIL_OUTPUT + std::string(PyUnicode_AsUTF8(repr)),
            true);
    Py_DECREF(repr);

    if (!result)
        throw Hooks::Exception("Datum was already defined in this script.");
}
