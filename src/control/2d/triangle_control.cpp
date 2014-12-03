#include <Python.h>

#include "control/2d/triangle_control.h"
#include "control/2d/point2d_control.h"

#include "graph/node/node.h"

TriangleControl::TriangleControl(Node* node, QObject* parent)
    : WireframeControl(node, parent),
      a(new Point2DControl(node->findChild<Node*>("a"), this)),
      b(new Point2DControl(node->findChild<Node*>("b"), this)),
      c(new Point2DControl(node->findChild<Node*>("c"), this))
{
    watchDatums({"a.x","a.y","b.x","b.y","c.x","c.y"});
}

QVector<QVector<QVector3D>> TriangleControl::lines() const
{
    float ax = a->getValue("x");
    float ay = a->getValue("y");
    float bx = b->getValue("x");
    float by = b->getValue("y");
    float cx = c->getValue("x");
    float cy = c->getValue("y");

    return {
        {QVector3D(ax, ay, 0), QVector3D(bx, by, 0), QVector3D(cx, cy, 0),
         QVector3D(ax, ay, 0)}};
}

void TriangleControl::drag(QVector3D center, QVector3D delta)
{
    a->drag(center, delta);
    b->drag(center, delta);
    c->drag(center, delta);
}

