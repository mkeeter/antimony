#include <Python.h>
#include <QDebug>

#include "datum/eval.h"
#include "datum/input.h"
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

PyObject* EvalDatum::validatePyObject(PyObject* v) const
{
    return v;
}

bool EvalDatum::validateExpr(QString e) const
{
    Q_UNUSED(e);
    return true;
}

PyObject* EvalDatum::validateType(PyObject* v) const
{
    if (v == NULL)
    {
        return NULL;
    } else if (PyObject_TypeCheck(v, getType())) {
        return v;
    }
    else
    {
        // Attempt to cast into the desired type.
        PyObject* out = PyObject_CallFunctionObjArgs((PyObject*)getType(), v, NULL);
        Py_DECREF(v);

        if (PyErr_Occurred())
        {
            Q_ASSERT(out == NULL);
            PyErr_Clear();
        }
        return out;
    }
}

int EvalDatum::getStartToken() const
{
    return Py_eval_input;
}

void EvalDatum::modifyGlobalsDict(PyObject *g)
{
    Q_UNUSED(g);
    // (nothing to do here; this function is mainly provided for the sake of
    //  the ScriptDatum derived class).
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

        if (PyErr_Occurred())
        {
            PyErr_Print();
            PyErr_Clear();
        }

        Py_DECREF(globals);
        Py_DECREF(locals);

        new_value = validatePyObject(validateType(new_value));
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

QString EvalDatum::getString() const
{
    if (hasInputValue())
    {
        return input_handler->getString();
    }
    else
    {
        return expr;
    }
}
