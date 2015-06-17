#include "graph/datum/datums/shape_datum.h"
#include "graph/datum/input.h"

ShapeDatum::ShapeDatum(QString name, Node* parent)
    : EvalDatum(name, parent)
{
    input_handler = new ShapeInputHandler(this);
    setExpr("None");
}
