#include <Python.h>

#include "graph/script.h"
#include "graph/node.h"
#include "graph/util.h"

#include <iostream>

Script::Script(Node* parent)
    : parent(parent), error_lineno(-1)
{
    // Nothing to do here
}

void Script::update()
{
    PyObject* globals = parent->proxyDict(this);
    active.clear();

    // Swap in a stringIO object for stdout, saving stdout in out
    PyObject* sys_mod = PyImport_ImportModule("sys");
    PyObject* io_mod = PyImport_ImportModule("io");
    PyObject* stdout_obj = PyObject_GetAttrString(sys_mod, "stdout");
    PyObject* stderr_obj = PyObject_GetAttrString(sys_mod, "stderr");
    PyObject* string_out = PyObject_CallMethod(io_mod, "StringIO", NULL);
    PyObject_SetAttrString(sys_mod, "stdout", string_out);
    PyObject_SetAttrString(sys_mod, "stderr", string_out);

    // Run the script
    PyObject* out = PyRun_String(
            script.c_str(), Py_file_input, globals, globals);
    Py_XDECREF(out);

    if (PyErr_Occurred())
    {
        auto err = getPyError();
        error = err.first;
        error_lineno = err.second;
        PyErr_Clear();
    }

    Py_DECREF(globals);

    // Get the output from the StringIO object
    PyObject* s = PyObject_CallMethod(string_out, "getvalue", NULL);
    output = std::string(PyUnicode_AsUTF8(s));

    // Swap stdout back into sys.stdout
    PyObject_SetAttrString(sys_mod, "stdout", stdout_obj);
    PyObject_SetAttrString(sys_mod, "stderr", stderr_obj);
    for (auto o : {sys_mod, io_mod, stdout_obj, stderr_obj, string_out, s})
        Py_DECREF(o);

    parent->update(active);
}
