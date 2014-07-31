#include "node/3d/cylinder_node.h"
#include "node/manager.h"

#include "datum/float_datum.h"
#include "datum/function_datum.h"

CylinderNode::CylinderNode(QObject* parent)
    : Node(parent)
{
    // Nothing to do here
}

CylinderNode::CylinderNode(float x, float y, float z, float scale,
                           QObject* parent)
    : Node(NodeManager::manager()->getName("c"), parent)
{
    new FloatDatum("x", QString::number(x), this);
    new FloatDatum("y", QString::number(y), this);
    new FloatDatum("z0", QString::number(z), this);
    new FloatDatum("z1", QString::number(z + scale), this);
    new FloatDatum("r", QString::number(scale / 2), this);
    new ShapeFunctionDatum("shape", this, "cylinder", {"x","y","z0","z1","r"});
}
