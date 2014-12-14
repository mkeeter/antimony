#include <Python.h>

#include "control/2d/triangle_control.h"
#include "control/2d/point2d_control.h"

#include "graph/node/node.h"

TriangleControl::TriangleControl(Node* node, QObject* parent)
    : WireframeControl(node, parent),
      a(new Point2DControl(node, this, "0")),
      b(new Point2DControl(node, this, "1")),
      c(new Point2DControl(node, this, "2"))
{
    watchDatums({"x0","x1","x2","y0","y1","y2"});
}

QVector<QVector<QVector3D>> TriangleControl::lines() const
{
    float ax = getValue("x0");
    float ay = getValue("y0");
    float bx = getValue("x1");
    float by = getValue("y1");
    float cx = getValue("x2");
    float cy = getValue("y2");

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

