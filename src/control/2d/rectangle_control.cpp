#include <Python.h>

#include "control/2d/rectangle_control.h"
#include "control/2d/point2d_control.h"

#include "graph/node/node.h"

RectangleControl::RectangleControl(Node* node, QObject* parent)
    : WireframeControl(node, parent),
      a(new Point2DControl(node->findChild<Node*>("a"), this)),
      b(new Point2DControl(node->findChild<Node*>("b"), this))
{
    watchDatums({"a.x","a.y","b.x","b.y"});
}

QVector<QVector<QVector3D>> RectangleControl::lines() const
{
    float ax = a->getValue("x");
    float ay = a->getValue("y");
    float bx = b->getValue("x");
    float by = b->getValue("y");

    return {
        {QVector3D(ax, ay, 0), QVector3D(ax, by, 0),
         QVector3D(bx, by, 0), QVector3D(bx, ay, 0),
         QVector3D(ax, ay, 0)}};
}

void RectangleControl::drag(QVector3D center, QVector3D delta)
{
    a->drag(center, delta);
    b->drag(center, delta);
}

