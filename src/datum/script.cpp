#include <Python.h>

#include "datum/script.h"
#include "datum/float.h"

#include "node/node.h"
#include "node/manager.h"

ScriptDatum::ScriptDatum(QString name, QString expr, QObject *parent)
    : Datum(name, parent)
{
    setScript(expr);
}

PyObject* ScriptDatum::getCurrentValue()
{
    globals = NodeManager::manager()->proxyDict(this);
    PyObject *locals = Py_BuildValue("{}");

    PyObject* out = PyRun_String(
                script.toStdString().c_str(),
                Py_file_input, globals, locals);
    PyObject_Print(out, stdout, 0);
    Py_DECREF(globals);
    Py_DECREF(locals);

    Py_DECREF(out);

    if (PyErr_Occurred())
    {
        return NULL;
    }

    Py_INCREF(Py_True);
    return Py_True;
}

void ScriptDatum::setScript(QString new_script)
{
    if (new_script != script)
    {
        script = new_script;
        update();
    }
}

void ScriptDatum::makeInput(QString name, PyTypeObject *type)
{
    Node* n = dynamic_cast<Node*>(parent());
    Datum* d = n->getDatum(name);

    if (d == NULL)
    {
        if (type == &PyFloat_Type)
        {
            d = new FloatDatum(name, "0.0", parent());
        }
        else
        {
            PyErr_SetString(PyExc_RuntimeError, "Invalid datum type");
        }
    }
    else if (d->getType() != type)
    {
        // delete old datum, make new datum
    }

    if (d->getValid())
    {
        PyDict_SetItemString(globals, name.toStdString().c_str(), d->getValue());
    }
    else
    {
        PyErr_SetString(PyExc_RuntimeError, "Accessed invalid datum value");
    }
}
