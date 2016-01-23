#include <QMatrix4x4>

#include "viewport/control/instance.h"
#include "viewport/control/control.h"
#include "viewport/view.h"

ControlInstance::ControlInstance(Control* c, ViewportView* v)
    : control(c), view(v)
{
    v->scene()->addItem(this);
}

QMatrix4x4 ControlInstance::getMatrix() const
{
    return view->getMatrix();
}

QRectF ControlInstance::boundingRect() const
{
    return control->bounds(getMatrix());
}

QPainterPath ControlInstance::shape() const
{
    return control->shape(getMatrix());
}

void ControlInstance::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem* option,
                         QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    control->paint(getMatrix(), isSelected(), painter);
}
