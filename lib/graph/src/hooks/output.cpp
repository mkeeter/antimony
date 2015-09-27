#include <Python.h>

#include "graph/hooks/output.h"
#include "graph/hooks/hooks.h"
#include "graph/script_node.h"
#include "graph/graph.h"

void OutputHook::call(std::string name, boost::python::object obj)
{
    Hooks::checkName(name);

    auto repr_ = PyObject_Repr(obj.ptr());
    if (PyErr_Occurred())
    {
        PyErr_Clear();
        throw Hooks::Exception("Failed to get __repr__ of argument");
    }
    auto repr = std::string(PyUnicode_AsUTF8(repr_));
    Py_DECREF(repr_);

    PyObject* g = Py_BuildValue(
            "{sO}", "__builtins__", PyEval_GetBuiltins());
    node->parent->loadDatumHooks(g);
    auto out = PyRun_String(repr.c_str(), Py_eval_input, g, g);
    Py_DECREF(g);
    Py_XDECREF(out);
    if (PyErr_Occurred())
    {
        PyErr_Clear();
        throw Hooks::Exception("Could not evaluate __repr__ of output");
    }

    const bool result = node->makeDatum(
            name, obj.ptr()->ob_type, Datum::SIGIL_OUTPUT + repr, true);

    if (!result)
        throw Hooks::Exception("Datum was already defined in this script.");
}
