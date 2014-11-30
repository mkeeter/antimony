#include "control/wireframe.h"

WireframeControl::WireframeControl(Node* node, QObject* parent)
    : Control(node, parent)
{
    // Nothing to do here
}

QPainterPath WireframeControl::shape(QMatrix4x4 m) const
{
    QPainterPathStroker stroker;
    stroker.setWidth(4);
    QPainterPath path = stroker.createStroke(linePath(m));
    path.addPath(pointPath(m));
    return path;
}

QRectF WireframeControl::bounds(QMatrix4x4 m) const
{
    return shape(m).boundingRect();
}

void WireframeControl::paint(QMatrix4x4 m, bool highlight, QPainter* painter)
{
    setDefaultPen(highlight, painter);
    painter->drawPath(linePath(m));
    setDefaultBrush(highlight, painter);
    painter->drawPath(pointPath(m));
}

QPainterPath WireframeControl::linePath(QMatrix4x4 m) const
{
    QPainterPath path;
    for (auto line : lines())
    {
        path.moveTo((m*line.front()).toPointF());
        for (auto pt : line)
            path.lineTo((m*pt).toPointF());
    }
    return path;
}

QPainterPath WireframeControl::pointPath(QMatrix4x4 m) const
{
    QPainterPath path;
    for (auto ptr : points())
    {
        QPointF pt = (m*ptr.first).toPointF();
        float r = ptr.second;
        path.addEllipse(QRectF(pt.x() - r, pt.y() - r, 2*r, 2*r));
    }
    return path;
}
