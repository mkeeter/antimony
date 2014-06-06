#include <Python.h>
#include <QDebug>

#include "datum/eval.h"

EvalDatum::EvalDatum(QString name, QObject *parent) :
    Datum(name, parent)
{
    // Nothing to do here
}

bool EvalDatum::validatePyObject(PyObject* v) const
{
    Q_UNUSED(v);
    return true;
}

bool EvalDatum::validateExpr(QString e) const
{
    Q_UNUSED(e);
    return true;
}

PyObject* EvalDatum::getCurrentValue() const
{
    PyObject *globals = Py_BuildValue("{}");
    PyObject *locals = Py_BuildValue("{}");

    QString e = prepareExpr(expr);
    PyObject* new_value = NULL;

    if (validateExpr(e))
    {
        new_value = PyRun_String(
                e.toStdString().c_str(), Py_eval_input, globals, locals);

        if (new_value == NULL)
        {
            PyErr_Clear();
        }

        Py_DECREF(globals);
        Py_DECREF(locals);

        if (new_value != NULL && !validatePyObject(new_value))
        {
            Py_DECREF(new_value);
            new_value = NULL;
        }
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
