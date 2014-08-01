#include "node/deform/repel_node.h"
#include "node/manager.h"

#include "datum/float_datum.h"
#include "datum/function_datum.h"
#include "datum/shape_datum.h"

RepelNode::RepelNode(QObject* parent)
    : Node(parent)
{
    // Nothing to do here
}

RepelNode::RepelNode(float x, float y, float z, float scale,
                     QObject* parent)
    : Node(NodeManager::manager()->getName("r"), parent)
{
    new FloatDatum("x", QString::number(x), this);
    new FloatDatum("y", QString::number(y), this);
    new FloatDatum("z", QString::number(z), this);
    new FloatDatum("r", QString::number(scale), this);
    new ShapeDatum("input", this);
    new ShapeFunctionDatum("shape", this, "repel",
            {"input", "x", "y", "z", "r"});
}
