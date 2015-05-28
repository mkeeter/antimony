#include "graph/datum/datums/float_output_datum.h"

FloatOutputDatum::FloatOutputDatum(QString name, Node* parent)
    : OutputDatum(name, parent)
{
    // Nothing to do here
}

PyTypeObject* FloatOutputDatum::getType() const
{
    return &PyFloat_Type;
}

QString FloatOutputDatum::getString() const
{
    if (valid)
        return QString::number(PyFloat_AsDouble(value));
    else
        return "Invalid output";
}
