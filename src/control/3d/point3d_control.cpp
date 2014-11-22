#include <Python.h>

#include <QPainter>

#include "graph/node/node.h"
#include "ui/canvas.h"
#include "control/3d/point3d_control.h"

Point3DControl::Point3DControl(Canvas* canvas, Node* node,
                               QGraphicsItem* parent)
    : WireframeControl(canvas, node, parent)
{
    watchDatums({"x","y","z"});
}

QVector3D Point3DControl::position() const
{
    return QVector3D(getValue("x"), getValue("y"), getValue("z"));
}


QVector<QPair<QVector3D, float>> Point3DControl::points() const
{
    return {{position(), 5}};
}

void Point3DControl::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(center);
    dragValue("x", delta.x());
    dragValue("y", delta.y());
    dragValue("z", delta.z());
}

QPointF Point3DControl::inspectorPosition() const
{
    return canvas->worldToScene(position());
}
