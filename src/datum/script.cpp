#include <Python.h>

#include "datum/script.h"
#include "datum/float.h"
#include "datum/wrapper.h"

#include "node/node.h"
#include "node/manager.h"


ScriptDatum::ScriptDatum(QString name, QString expr, QObject *parent)
    : EvalDatum(name, parent), globals(NULL),
      input_func(scriptInput(this)), output_func(scriptOutput(this))
{
    setExpr(expr);
}

ScriptDatum::~ScriptDatum()
{
    Py_DECREF(input_func);
    Py_DECREF(output_func);
}

int ScriptDatum::getStartToken() const
{
    return Py_file_input;
}

void ScriptDatum::modifyGlobalsDict(PyObject* g)
{
    globals = g;
    PyDict_SetItemString(g, "input", input_func);
    PyDict_SetItemString(g, "output", output_func);
}

PyObject* ScriptDatum::makeInput(QString name, PyTypeObject *type)
{
    Node* n = dynamic_cast<Node*>(parent());
    Datum* d = n->getDatum(name);

    if (d != NULL && d->getType() != type)
    {
        d->deleteLater();
        d = NULL;
    }

    if (d == NULL)
    {
        if (type == &PyFloat_Type)
        {
            d = new FloatDatum(name, "0.0", parent());
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "Invalid datum type");
            return NULL;
        }
    }

    if (d->getValid())
    {
        PyDict_SetItemString(globals, name.toStdString().c_str(), d->getValue());
    }
    else
    {
        PyErr_SetString(PyExc_RuntimeError, "Accessed invalid datum value");
        return NULL;
    }
    return Py_None;
}
