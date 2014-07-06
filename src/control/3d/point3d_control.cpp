#include <Python.h>

#include <QPainter>

#include "node/node.h"
#include "ui/canvas.h"
#include "control/3d/point3d_control.h"

Point3DControl::Point3DControl(Canvas* canvas, Node* node,
                               QGraphicsItem* parent)
    : Control(canvas, node, parent)
{
    watchDatums({"x","y","z"});
}

QVector3D Point3DControl::position() const
{
    return QVector3D(getValue("x"), getValue("y"), getValue("z"));
}

QRectF Point3DControl::boundingRect() const
{
    return node ? boundingBox({position()}) : QRectF();
}

void Point3DControl::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    setDefaultBrush(painter);
    setDefaultPen(painter);
    painter->drawPath(shape());
}

QPainterPath Point3DControl::shape() const
{
    QPointF pt = canvas->worldToScene(position());
    QPainterPath path;
    path.addEllipse(pt.x() - 5, pt.y() - 5, 10, 10);
    return path;
}

void Point3DControl::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(center);
    dragValue("x", delta.x());
    dragValue("y", delta.y());
    dragValue("z", delta.z());
    prepareGeometryChange();
}

QPointF Point3DControl::inspectorPosition() const
{
    return canvas->worldToScene(position());
}
