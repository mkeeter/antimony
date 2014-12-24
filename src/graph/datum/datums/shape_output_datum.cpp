#include "graph/datum/datums/shape_output_datum.h"
#include "fab/fab.h"

ShapeOutputDatum::ShapeOutputDatum(QString name, Node* parent)
    : OutputDatum(name, parent)
{
    // Nothing to do here
}

PyTypeObject* ShapeOutputDatum::getType() const
{
    return fab::ShapeType;
}
