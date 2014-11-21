#include <Python.h>
#include "graph/datum/types/output_datum.h"
#include "fab/fab.h"

OutputDatum::OutputDatum(QString name, QObject* parent)
    : Datum(name, parent), new_value(NULL)
{
    // Nothing to do here
}

OutputDatum::~OutputDatum()
{
    Py_XDECREF(new_value);
}

void OutputDatum::setNewValue(PyObject *p)
{
    Q_ASSERT(p);

    Py_INCREF(p);
    Py_XDECREF(new_value);
    new_value = p;
    update();
}

PyObject* OutputDatum::getCurrentValue()
{
    Py_XINCREF(new_value);
    return new_value;
}

QString OutputDatum::getString() const
{
    return "Output";
}

////////////////////////////////////////////////////////////////////////////////

ShapeOutputDatum::ShapeOutputDatum(QString name, QObject* parent)
    : OutputDatum(name, parent)
{
    // Nothing to do here
}

PyTypeObject* ShapeOutputDatum::getType() const
{
    return fab::ShapeType;
}
