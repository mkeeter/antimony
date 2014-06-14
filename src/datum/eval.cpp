#include <Python.h>
#include <QDebug>

#include "datum/eval.h"
#include "node/manager.h"

EvalDatum::EvalDatum(QString name, QObject *parent) :
    Datum(name, parent)
{
    // Nothing to do here
}

QString EvalDatum::prepareExpr(QString s) const
{
    return s;
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

bool EvalDatum::validateType(PyObject* v) const
{
    return PyObject_TypeCheck(v, getType());
}

int EvalDatum::getStartToken() const
{
    return Py_eval_input;
}

void EvalDatum::modifyGlobalsDict(PyObject *g)
{
    // Nothing to do here
}

PyObject* EvalDatum::getCurrentValue()
{
    QString e = prepareExpr(expr);
    PyObject* new_value = NULL;

    if (validateExpr(e))
    {
        PyObject* globals = NodeManager::manager()->proxyDict(this);
        PyObject* locals = Py_BuildValue("{}");

        modifyGlobalsDict(globals);

        new_value = PyRun_String(
                 e.toStdString().c_str(),
                 getStartToken(), globals, locals);

        if (new_value == NULL)
        {
            PyErr_Print();
            PyErr_Clear();
        }

        Py_DECREF(globals);
        Py_DECREF(locals);

        if (new_value != NULL && (!validateType(new_value) ||
                                  !validatePyObject(new_value)))
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
