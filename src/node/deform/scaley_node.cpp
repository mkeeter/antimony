#include "node/deform/scaley_node.h"
#include "node/manager.h"

#include "datum/float_datum.h"
#include "datum/function_datum.h"
#include "datum/shape_datum.h"

ScaleYNode::ScaleYNode(QObject* parent)
    : Node(parent)
{
    // Nothing to do here
}

ScaleYNode::ScaleYNode(float x, float y, float z, float scale,
                     QObject* parent)
    : Node(NodeManager::manager()->getName("s"), parent)
{
    new FloatDatum("_x", QString::number(x), this);
    new FloatDatum("y", QString::number(y), this);
    new FloatDatum("_z", QString::number(z), this);
    new FloatDatum("s", QString::number(scale), this);
    new ShapeDatum("input", this);
    new ShapeFunctionDatum("shape", this, "scale_y",
            {"input", "y", "s"});
}
