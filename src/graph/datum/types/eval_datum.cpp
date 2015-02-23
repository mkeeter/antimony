#include <Python.h>
#include <QDebug>

#include "graph/datum/types/eval_datum.h"
#include "graph/datum/input.h"

#include "graph/node/root.h"

EvalDatum::EvalDatum(QString name, Node* parent) :
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
        return NULL;
    else if (PyObject_TypeCheck(v, getType()))
        return v;

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

void EvalDatum::modifyGlobalsDict(PyObject* g)
{
    Q_UNUSED(g);
    // (nothing to do here; this function is mainly provided for the sake of
    //  the ScriptDatum derived class).
}

PyObject* EvalDatum::getCurrentValue()
{
    error_lineno = -1;
    error_traceback = "";

    QString e = prepareExpr(expr);
    PyObject* new_value = NULL;

    if (validateExpr(e))
    {
        PyObject* globals = root()->proxyDict(this);

        modifyGlobalsDict(globals);

        new_value = PyRun_String(
                 e.toStdString().c_str(),
                 getStartToken(), globals, globals);

        if (PyErr_Occurred())
        {
            onPyError();
            PyErr_Clear();
        }

        Py_DECREF(globals);

        new_value = validatePyObject(validateType(new_value));
    }
    return new_value;
}

void EvalDatum::setExpr(QString new_expr)
{
    // If the expression has changed or this is the first call to
    // set_expr, then run update().
    if (new_expr != expr || !post_init_called)
    {
        expr = new_expr;
        update();
    }
}

QString EvalDatum::getString() const
{
    if (hasInputValue())
        return input_handler->getString();
    else
        return expr;
}

void EvalDatum::onPyError()
{
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
    Q_ASSERT(tbmod);

    PyObject* format_func = PyObject_GetAttrString(tbmod, "format_exception");
    Q_ASSERT(format_func);

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
        wchar_t* w = PyUnicode_AsWideCharString(
                PyList_GetItem(lst, i), NULL);
        Q_ASSERT(w);
        error_traceback += QString::fromWCharArray(w);
        PyMem_Free(w);
    }

    // Chop off the trailing "\n"
    error_traceback.chop(1);

    // ...and clean up all of the Python objects.
    Py_DECREF(args);
    Py_DECREF(tbmod);
    Py_DECREF(lst);
    Py_DECREF(format_func);

    Py_XDECREF(ptype);
    Py_XDECREF(pvalue);
    Py_XDECREF(ptraceback);
}
