#include "node/2d/triangle_node.h"
#include "node/2d/point2d_node.h"

#include "node/manager.h"

#include "datum/function_datum.h"

TriangleNode::TriangleNode(QObject* parent)
    : Node(parent)
{
    // Nothing to do here
}

TriangleNode::TriangleNode(float x, float y, float z, float scale,
                           QObject* parent)
    : Node(NodeManager::manager()->getName("t"), parent)
{
    Node* a = new Point2D(x, y, z, 0, this);
    Node* b = new Point2D(x + scale, y, z, 0, this);
    Node* c = new Point2D(x, y + scale, z, 0, this);
    a->setObjectName("a");
    b->setObjectName("b");
    c->setObjectName("c");
    new ShapeFunctionDatum("shape", this, "triangle",
            {"a.x","a.y","b.x","b.y","c.x","c.y"});
}
