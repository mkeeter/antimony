#include <Python.h>

#include "control/2d/triangle_control.h"
#include "control/2d/point2d_control.h"

#include "node/node.h"
#include "ui/canvas.h"

TriangleControl::TriangleControl(Canvas* canvas, Node* node)
    : WireframeControl(canvas, node),
      a(new Point2DControl(canvas, node->findChild<Node*>("a"), this)),
      b(new Point2DControl(canvas, node->findChild<Node*>("b"), this)),
      c(new Point2DControl(canvas, node->findChild<Node*>("c"), this))
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

QPointF TriangleControl::inspectorPosition() const
{
    return canvas->worldToScene(QVector3D(
            (a->getValue("x") + b->getValue("x")) / 2,
            (a->getValue("y") + b->getValue("y")) / 2,
            0));
}
