#include "node/deform/attract_node.h"
#include "node/manager.h"

#include "datum/float_datum.h"
#include "datum/function_datum.h"
#include "datum/shape_datum.h"

AttractNode::AttractNode(QObject* parent)
    : Node(parent)
{
    // Nothing to do here
}

AttractNode::AttractNode(float x, float y, float z, float scale,
                     QObject* parent)
    : Node(NodeManager::manager()->getName("a"), parent)
{
    new FloatDatum("x", QString::number(x), this);
    new FloatDatum("y", QString::number(y), this);
    new FloatDatum("z", QString::number(z), this);
    new FloatDatum("r", QString::number(scale), this);
    new ShapeDatum("input", this);
    new ShapeFunctionDatum("shape", this, "attract",
            {"input", "x", "y", "z", "r"});
}
