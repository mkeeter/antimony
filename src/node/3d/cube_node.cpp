#include "node/3d/cube_node.h"
#include "node/3d/point3d_node.h"
#include "node/manager.h"

#include "datum/float_datum.h"
#include "datum/function_datum.h"

CubeNode::CubeNode(QString name, QObject* parent)
    : _Node(name, parent)
{
    // Nothing to do here
}

CubeNode::CubeNode(float x, float y, float z, float scale, QObject *parent)
    : CubeNode(NodeManager::manager()->getName("c"), parent)
{
    a = new Point3D(x, y, z, 0, this);
    b = new Point3D(x + scale, y + scale, z + scale, 0, this);
    a->setObjectName("a");
    b->setObjectName("b");
    new ShapeFunctionDatum("shape", this, "cube",
        {"a.x", "b.x", "a.y", "b.y", "a.z", "b.z"});
}
