#include <Python.h>

#include "control/2d/rectangle_control.h"
#include "control/2d/point2d_control.h"

#include "graph/node/node.h"

RectangleControl::RectangleControl(Node* node, QObject* parent)
    : WireframeControl(node, parent),
      a(new Point2DControl(node, this, "min")),
      b(new Point2DControl(node, this, "max"))
{
    watchDatums({"xmin","ymin","xmax","ymax"});
}

QVector<QVector<QVector3D>> RectangleControl::lines() const
{
    float ax = getValue("xmin");
    float ay = getValue("ymin");
    float bx = getValue("xmax");
    float by = getValue("ymax");

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

