#include <Python.h>
#include "datum/name.h"
#include "datum/input.h"
#include <QDebug>

PyObject* NameDatum::kwlist = NULL;

NameDatum::NameDatum(QString name, QString expr, QObject *parent)
    : EvalDatum(name, parent)
{
    setExpr(expr);

    // Lazy initialization of keyword list.
    if (!kwlist)
    {
        PyObject *globals = Py_BuildValue("{}");
        PyObject *locals = Py_BuildValue("{}");
        kwlist = PyRun_String("__import__('keyword').kwlist",
                              Py_eval_input, globals, locals);
        Py_DECREF(globals);
        Py_DECREF(locals);
    }
}

bool NameDatum::validate(PyObject *v) const
{
    return PyFloat_CheckExact(v);
}
