#include "node/3d/sphere_node.h"
#include "node/manager.h"

#include "datum/float_datum.h"
#include "datum/function_datum.h"

SphereNode::SphereNode(QObject* parent)
    : Node(parent)
{
    // Nothing to do here
}


SphereNode::SphereNode(float x, float y, float z, float scale,
                       QObject* parent)
    : Node(NodeManager::manager()->getName("c"), parent)
{
    new FloatDatum("x", QString::number(x), this);
    new FloatDatum("y", QString::number(y), this);
    new FloatDatum("z", QString::number(z), this);
    new FloatDatum("r", QString::number(scale), this);
    new ShapeFunctionDatum("shape", this, "sphere", {"x", "y", "z", "r"});
}
