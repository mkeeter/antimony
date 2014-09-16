#include "control/variable/slider_control.h"

#include "ui/canvas.h"
#include "ui/colors.h"

SliderHandle::SliderHandle(Canvas* canvas, Node* node,
                             QGraphicsItem* parent)
    : Control(canvas, node, parent)
{
    watchDatums({"_x","_y","_z","min","max","value"});
}

QRectF SliderHandle::bounds() const
{
    QPointF p = position();
    return QRectF(p.x() - 4, p.y() - 10, 8, 20);
}

void SliderHandle::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(delta);
    QPointF base = canvas->worldToScene(QVector3D(getValue("_x"),
                                                  getValue("_y"),
                                                  getValue("_z")));
    float dx = 2 * canvas->getScale();
    float frac = (canvas->worldToScene(center).x() - base.x()) / dx;
    if (frac < 0)       frac = 0;
    else if (frac > 1)  frac = 1;

    setValue("value", getValue("min") + frac * (getValue("max") - getValue("min")));
}

QPointF SliderHandle::position() const
{
    return canvas->worldToScene(QVector3D(getValue("_x"),
                                          getValue("_y"),
                                          getValue("_z"))) +
        QPointF(2 * canvas->getScale() * (getValue("value") - getValue("min")) /
                                         (getValue("max") - getValue("min")), 2);
}

void SliderHandle::paintControl(QPainter* painter)
{
    painter->setPen(Qt::NoPen);

    if (isSelected() || _hover)
    {
        painter->setBrush(Colors::highlight(Colors::yellow));
    } else
    {
        painter->setBrush(Colors::yellow);
    }
    painter->drawRect(bounds());

    QPointF p = position();
    if (isSelected() || _hover)
    {
        painter->setBrush(Colors::yellow);
    } else
    {
        painter->setBrush(Colors::dim(Colors::yellow));
    }
    painter->drawRect(p.x() + 2, p.y() - bounds().height() / 2,
                      2, bounds().height());
}


////////////////////////////////////////////////////////////////////////////////

SliderControl::SliderControl(Canvas* canvas, Node* node)
    : Control(canvas, node), handle(new SliderHandle(canvas, node, this))
{
    watchDatums({"_x","_y","_z","min","max","value"});
}

QRectF SliderControl::bounds() const
{
    QPointF p = position();
    return QRectF(p.x(), p.y() - 5, 2 * canvas->getScale(), 10);
}

void SliderControl::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(center);
    dragValue("_x", delta.x());
    dragValue("_y", delta.y());
    dragValue("_z", delta.z());
}

QPointF SliderControl::position() const
{
    return canvas->worldToScene(QVector3D(getValue("_x"),
                                          getValue("_y"),
                                          getValue("_z")));
}

void SliderControl::paintControl(QPainter* painter)
{
    painter->setPen(Qt::NoPen);

    if (isSelected() || _hover)
    {
        painter->setBrush(Colors::highlight(Colors::yellow));
    } else
    {
        painter->setBrush(Colors::yellow);
    }
    QPointF p = position();
    QRectF b = bounds();

    painter->drawRect(p.x(), p.y() - 2, b.width(), 4);

    if (isSelected() || _hover)
    {
        painter->setBrush(Colors::yellow);
    } else
    {
        painter->setBrush(Colors::dim(Colors::yellow));
    }
    painter->drawRect(p.x(), p.y(), b.width(), 2);

    // Draw ends on the bar
    painter->drawRect(b.left(), b.top(), 4, b.height());
    painter->drawRect(b.right() - 4, b.top(), 4, b.height());
}

QPointF SliderControl::inspectorPosition() const
{
    return QPointF(bounds().right(), bounds().center().y());
}


QPointF SliderControl::baseInputPosition() const
{
    return QPointF(bounds().left(), bounds().center().y());
}
