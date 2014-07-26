#include <Python.h>

#include "datum/function_datum.h"
#include "node/node.h"
#include "cpp/fab.h"

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

    Py_DECREF(args);
    return out;
}

QString FunctionDatum::getString() const
{
    return function_name;
}

////////////////////////////////////////////////////////////////////////////////

ShapeFunctionDatum::ShapeFunctionDatum(QString name, QObject* parent)
    : FunctionDatum(name, parent)
{
    // Nothing to do here
}

ShapeFunctionDatum::ShapeFunctionDatum(QString name, QObject* parent,
                                       QString func_name, QList<QString> args)
    : FunctionDatum(name, parent, func_name, args)
{
    update();
}

PyObject* ShapeFunctionDatum::getModule() const
{
    PyObject* fab_str = PyUnicode_FromString("fab");
    PyObject* fab = PyImport_Import(fab_str);

    PyObject* shapes = PyObject_GetAttrString(fab, "shapes");
    Py_DECREF(fab);
    Py_DECREF(fab_str);

    return shapes;
}

