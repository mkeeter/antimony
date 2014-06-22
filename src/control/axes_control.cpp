#include <QVector3D>

#include "control/axes_control.h"
#include "control/control.h"
#include "ui/canvas.h"

AxesControl::AxesControl(Canvas* canvas) :
    DummyControl(canvas, NULL)
{
    // Nothing to do here
}

QRectF AxesControl::boundingRect() const
{
    return boundingBox({
            QVector3D(0, 0, 0),
            QVector3D(100, 0, 0),
            QVector3D(0, 100, 0),
            QVector3D(0, 0, 100)});
}

void AxesControl::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QVector<QPointF> pts = canvas->worldToScene({
            QVector3D(0, 0, 0),
            QVector3D(100, 0, 0),
            QVector3D(0, 100, 0),
            QVector3D(0, 0, 100)});

    painter->setPen(QPen(Qt::red, 2));
    painter->drawLine(pts[0], pts[1]);
    painter->setPen(QPen(Qt::green, 2));
    painter->drawLine(pts[0], pts[2]);
    painter->setPen(QPen(Qt::blue, 2));
    painter->drawLine(pts[0], pts[3]);
}
