#include <Python.h>
#include <QDebug>

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

    PyObject* new_value = PyRun_String(
            expr.toStdString().c_str(), Py_eval_input, globals, locals);

    if (new_value == NULL)
    {
        PyErr_Clear();
    }

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
