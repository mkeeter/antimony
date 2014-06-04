#include <Python.h>
#include "datum/eval.h"

EvalDatum::EvalDatum(QString name, QObject *parent) :
    Datum(name, parent)
{
    // Nothing to do here
}

PyObject* EvalDatum::getValue() const
{
    PyObject *globals = Py_BuildValue("{}");
    PyObject *locals = Py_BuildValue("{}");
    const char* a = "123.3";
    PyObject* new_value = PyRun_String(
                a, Py_file_input, globals, locals);
    Py_DECREF(globals);
    Py_DECREF(locals);

    if (new_value != NULL && !validate(new_value))
    {
        Py_DECREF(new_value);
        new_value = NULL;
    }
    return new_value;
}

void EvalDatum::setExpr(QString new_expr)
{
    if (new_expr != expr)
    {
        expr = new_expr;
        update();
    }
}
