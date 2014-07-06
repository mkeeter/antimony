#include <Python.h>

#include "control/multiline.h"
#include "node/node.h"
#include "ui/canvas.h"

MultiLineControl::MultiLineControl(Canvas* canvas, Node* node,
                                   QGraphicsItem* parent)
    : Control(canvas, node, parent)
{
    // Nothing to do here
}

QRectF MultiLineControl::bounds() const
{
    QVector<QVector3D> points;
    for (auto line : lines())
    {
        points << line;
    }
    return boundingBox(points);
}

QPainterPath MultiLineControl::path() const
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

QPainterPath MultiLineControl::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(4);
    return stroker.createStroke(path());
}


void MultiLineControl::paintControl(QPainter *painter)
{
    setDefaultPen(painter);
    painter->drawPath(path());
}
