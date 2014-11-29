#include <QVector3D>

#include "control/axes_control.h"
#include "control/control.h"

#include "ui/canvas.h"
#include "ui/colors.h"

AxesControl::AxesControl(Canvas* canvas) :
    DummyControl(canvas, NULL)
{
    setZValue(-10);
}

QRectF AxesControl::boundingRect() const
{
    return boundingBox({
            QVector3D(0, 0, 0),
            QVector3D(1, 0, 0),
            QVector3D(0, 1, 0),
            QVector3D(0, 0, 1)});
}

QRectF AxesControl::bounds(QMatrix4x4 m) const
{
#error "Not yet implemented"
}

void AxesControl::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QVector<QPointF> pts = canvas->worldToScene({
            QVector3D(0, 0, 0),
            QVector3D(1, 0, 0),
            QVector3D(0, 1, 0),
            QVector3D(0, 0, 1)});

    painter->setPen(QPen(Colors::red, 2));
    painter->drawLine(pts[0], pts[1]);
    painter->setPen(QPen(Colors::green, 2));
    painter->drawLine(pts[0], pts[2]);
    painter->setPen(QPen(Colors::blue, 2));
    painter->drawLine(pts[0], pts[3]);
}
