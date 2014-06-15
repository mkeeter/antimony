#include "datum/shape_datum.h"
#include "datum/input.h"

MathShapeDatum::MathShapeDatum(QString name, QObject* parent)
    : Datum(name, parent)
{
    input_handler = new ShapeInputHandler(this);
}
