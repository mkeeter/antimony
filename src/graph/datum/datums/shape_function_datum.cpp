#include "graph/datum/datums/shape_function_datum.h"
#include "fab/fab.h"

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
    PyObject* fab = PyImport_ImportModule("fab");

    PyObject* shapes = PyObject_GetAttrString(fab, "shapes");
    Py_DECREF(fab);

    return shapes;
}

PyTypeObject* ShapeFunctionDatum::getType() const
{
    return fab::ShapeType;
}
