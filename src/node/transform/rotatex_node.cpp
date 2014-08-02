#include "node/transform/rotatex_node.h"
#include "node/manager.h"

#include "datum/float_datum.h"
#include "datum/function_datum.h"
#include "datum/shape_datum.h"

RotateXNode::RotateXNode(QObject* parent)
    : Node(parent)
{
    // Nothing to do here
}

RotateXNode::RotateXNode(float x, float y, float z, float scale,
                     QObject* parent)
    : Node(NodeManager::manager()->getName("r"), parent)
{
    Q_UNUSED(scale);
    new FloatDatum("_x", QString::number(x), this);
    new FloatDatum("y", QString::number(y), this);
    new FloatDatum("z", QString::number(z), this);
    new FloatDatum("a", QString::number(45), this);
    new ShapeDatum("input", this);
    new ShapeFunctionDatum("shape", this, "rotate_x",
            {"input", "a", "y", "z"});
}
