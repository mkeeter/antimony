#include <Python.h>

#include "graph/datum/types/function_datum.h"
#include "graph/node/node.h"

FunctionDatum::FunctionDatum(QString name, QObject* parent)
    : Datum(name, parent), function(NULL)
{
    // Nothing to do here
}

FunctionDatum::FunctionDatum(QString name, QObject* parent,
                             QString f, QList<QString> args)
    : Datum(name, parent), function_name(f), arguments(args),
      function(NULL)
{
    // Child needs to call update(), but we can't do that here
}

FunctionDatum::~FunctionDatum()
{
    Py_XDECREF(function);
}

PyObject* FunctionDatum::getCurrentValue()
{
    // Lazy initialization of function PyObject*
    if (function == NULL)
    {
        PyObject* module = getModule();
        function = PyObject_GetAttrString(
                    module, function_name.toStdString().c_str());
        Py_DECREF(module);
    }

    // Make a tuple containing all of the argument values
    bool success = true;
    int count = 0;
    PyObject* args = PyTuple_New(arguments.length());
    for (auto a : arguments)
    {
        Datum* d = dynamic_cast<Node*>(parent())->getDatum(a);

        if (d)
        {
            if (connectUpstream(d) && d->getValid())
            {
                PyObject* v = d->getValue();
                Py_INCREF(v);
                PyTuple_SetItem(args, count++, v);
            }
            else
            {
                success = false;
            }
        }
        else
        {
            success = false;
        }
    }

    PyObject* out = success ? PyObject_CallObject(function, args) : NULL;

    if (PyErr_Occurred())
    {
        PyErr_Print();
    }
    Py_DECREF(args);
    return out;
}

QString FunctionDatum::getString() const
{
    return function_name;
}

void FunctionDatum::setFunction(QString f, QList<QString> args)
{
    function_name = f;
    arguments = args;
    update();
}
