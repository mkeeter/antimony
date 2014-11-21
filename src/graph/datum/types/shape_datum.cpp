#include "graph/datum/types/shape_datum.h"
#include "graph/datum/input.h"

ShapeDatum::ShapeDatum(QString name, QObject* parent)
    : Datum(name, parent)
{
    input_handler = new ShapeInputHandler(this);
    update();
}

QString ShapeDatum::getString() const
{
    return input_handler->getString();
}
