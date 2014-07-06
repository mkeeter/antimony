#include <Python.h>

#include <QPainter>

#include "ui/canvas.h"
#include "control/2d/point2d_control.h"

Point2DControl::Point2DControl(Canvas* canvas, Node* node, QGraphicsItem* parent)
    : Control(canvas, node, parent)
{
    watchDatums({"x","y"});
}

QVector3D Point2DControl::position() const
{
    return QVector3D(getValue("x"), getValue("y"), 0);
}

QRectF Point2DControl::bounds() const
{
    return boundingBox({position()});
}

void Point2DControl::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
    if (node.isNull())
    {
        return;
    }

    Q_UNUSED(option);
    Q_UNUSED(widget);
    setDefaultBrush(painter);
    setDefaultPen(painter);
    painter->drawPath(shape());
}

QPainterPath Point2DControl::shape() const
{
    QPointF pt = canvas->worldToScene(position());
    QPainterPath path;
    path.addEllipse(pt.x() - 5, pt.y() - 5, 10, 10);
    return path;
}

void Point2DControl::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(center);
    dragValue("x", delta.x());
    dragValue("y", delta.y());
    prepareGeometryChange();
}

QPointF Point2DControl::inspectorPosition() const
{
    return canvas->worldToScene(position());
}
