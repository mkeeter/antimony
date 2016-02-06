#include <Python.h>

#include <cassert>
#include <iostream>

#include "graph/util.h"

std::pair<std::string, int> getPyError()
{
    std::string error_traceback;
    int error_lineno = -1;

    PyObject *ptype, *pvalue, *ptraceback;
    PyErr_Fetch(&ptype, &pvalue, &ptraceback);

    // Extract the error's line number, with special case
    // for when we aren't given a traceback.
    if (ptraceback)
    {
        PyObject* lineno = PyObject_GetAttrString(ptraceback, "tb_lineno");
        error_lineno = PyLong_AsLong(lineno);
        Py_DECREF(lineno);
    } else {
        error_lineno = PyLong_AsLong(PyTuple_GetItem(
                                     PyTuple_GetItem(pvalue, 1), 1));
    }

    // Call traceback.format_exception on the traceback.
    PyErr_NormalizeException(&ptype, &pvalue, &ptraceback);

    PyObject* tbmod = PyImport_ImportModule("traceback");
    assert(tbmod);

    PyObject* format_func = PyObject_GetAttrString(tbmod, "format_exception");
    assert(format_func);

    if (!ptraceback)
    {
        ptraceback = Py_None;
        Py_INCREF(Py_None);
    }

    PyObject* args = Py_BuildValue("(OOO)", ptype, pvalue, ptraceback);
    PyObject* lst = PyObject_CallObject(format_func, args);

    // Concatenate the traceback list into a QString.
    error_traceback = "";
    for (int i=0; i < PyList_Size(lst); ++i)
    {
        char* c = PyUnicode_AsUTF8(PyList_GetItem(lst, i));
        assert(!PyErr_Occurred());
        error_traceback += std::string(c);
    }

    // Chop off the trailing "\n"
    error_traceback.pop_back();

    // ...and clean up all of the Python objects.
    for (auto a : {args, tbmod, lst, format_func})
        Py_DECREF(a);

    for (auto a : {ptype, pvalue, ptraceback})
        Py_XDECREF(a);

    return std::make_pair(error_traceback, error_lineno);
}



bool isPyKeyword(std::string k)
{
    // Lazy initialization of keyword.kwlist.__contains__
    static PyObject* kwlist_contains = NULL;
    if (!kwlist_contains)
    {
        PyObject* keyword_module = PyImport_ImportModule("keyword");
        PyObject* kwlist = PyObject_GetAttrString(keyword_module, "kwlist");
        Py_DECREF(keyword_module);
        kwlist_contains = PyObject_GetAttrString(kwlist, "__contains__");
        Py_DECREF(kwlist);
    }

    PyObject* args = Py_BuildValue("(s)", k.c_str());
    PyObject* in_kwlist = PyObject_Call(kwlist_contains, args, NULL);
    Py_DECREF(args);

    const bool result = PyObject_IsTrue(in_kwlist);
    Py_DECREF(in_kwlist);

    return result;
}
