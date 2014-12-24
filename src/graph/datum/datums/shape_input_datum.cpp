#include "graph/datum/datums/shape_input_datum.h"
#include "graph/datum/input.h"

#include "fab/fab.h"

ShapeInputDatum::ShapeInputDatum(QString name, Node* parent)
    : InputDatum(name, parent)
{
    input_handler = new ShapeInputHandler(this);
    update();
}

PyTypeObject* ShapeInputDatum::getType() const
{
    return fab::ShapeType;
}
