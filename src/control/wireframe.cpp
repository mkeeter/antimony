#include <Python.h>

#include "control/wireframe.h"
#include "ui/canvas.h"

WireframeControl::WireframeControl(Canvas* canvas, Node* node,
                                   QGraphicsItem* parent)
    : Control(canvas, node, parent)
{
    // Nothing to do here
}

QPainterPath WireframeControl::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(4);
    QPainterPath path = stroker.createStroke(linePath());
    path.addPath(pointPath());
    return path;
}

QRectF WireframeControl::bounds() const
{
    return shape().boundingRect();
}

void WireframeControl::paintControl(QPainter* painter)
{
    setDefaultPen(painter);
    painter->drawPath(linePath());
    setDefaultBrush(painter);
    painter->drawPath(pointPath());
}

QPainterPath WireframeControl::linePath() const
{
    QPainterPath path;
    for (auto line : lines())
    {
        auto pts = canvas->worldToScene(line);
        path.moveTo(pts.front());
        for (auto pt : pts)
        {
            path.lineTo(pt);
        }
    }
    return path;
}

QPainterPath WireframeControl::pointPath() const
{
    QPainterPath path;
    for (auto ptr : points())
    {
        QPointF pt = canvas->worldToScene(ptr.first);
        float r = ptr.second;
        path.addEllipse(QRectF(pt.x() - r, pt.y() - r, 2*r, 2*r));
    }
    return path;
}
