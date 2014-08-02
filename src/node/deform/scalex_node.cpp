#include "node/deform/scalex_node.h"
#include "node/manager.h"

#include "datum/float_datum.h"
#include "datum/function_datum.h"
#include "datum/shape_datum.h"

ScaleXNode::ScaleXNode(QObject* parent)
    : Node(parent)
{
    // Nothing to do here
}

ScaleXNode::ScaleXNode(float x, float y, float z, float scale,
                     QObject* parent)
    : Node(NodeManager::manager()->getName("s"), parent)
{
    new FloatDatum("x", QString::number(x), this);
    new FloatDatum("_y", QString::number(y), this);
    new FloatDatum("_z", QString::number(z), this);
    new FloatDatum("s", QString::number(scale), this);
    new ShapeDatum("input", this);
    new ShapeFunctionDatum("shape", this, "scale_x",
            {"input", "x", "s"});
}
