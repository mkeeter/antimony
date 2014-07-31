#include "node/3d/extrude_node.h"
#include "node/manager.h"

#include "datum/float_datum.h"
#include "datum/function_datum.h"
#include "datum/shape_datum.h"

ExtrudeNode::ExtrudeNode(QObject* parent)
    : Node(parent)
{
    // Nothing to do here
}

ExtrudeNode::ExtrudeNode(float x, float y, float z, float scale,
                         QObject* parent)
    : Node(NodeManager::manager()->getName("e"), parent)
{
    new FloatDatum("_x", QString::number(x), this);
    new FloatDatum("_y", QString::number(y), this);
    new FloatDatum("z0", QString::number(z), this);
    new FloatDatum("z1", QString::number(z + scale), this);
    new FloatDatum("_scale", QString::number(scale/8), this);
    new ShapeDatum("input", this);
    new ShapeFunctionDatum("shape", this, "extrude_z", {"input","z0","z1"});
}
