#include <Python.h>

#include <QPainter>

#include "ui/canvas.h"
#include "control/2d/point2d_control.h"

Point2DControl::Point2DControl(Canvas* canvas, Node* node, QGraphicsItem* parent)
    : WireframeControl(canvas, node, parent)
{
    watchDatums({"x","y"});
}

QVector<QPair<QVector3D, float>> Point2DControl::points() const
{
    return {{position(), 5}};
}

QVector3D Point2DControl::position() const
{
    return QVector3D(getValue("x"), getValue("y"), 0);
}

void Point2DControl::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(center);
    dragValue("x", delta.x());
    dragValue("y", delta.y());
}

QPointF Point2DControl::inspectorPosition() const
{
    return canvas->worldToScene(position());
}
