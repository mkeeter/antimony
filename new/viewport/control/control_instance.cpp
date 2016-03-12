#include <Python.h>

#include <QMatrix4x4>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

#include "viewport/control/control_instance.h"
#include "viewport/control/control.h"
#include "viewport/view.h"

ControlInstance::ControlInstance(Control* c, ViewportView* v)
    : control(c), view(v)
{
    setFlags(QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(true);

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

    control->paint(getMatrix(), isSelected() || hover, painter);
}

////////////////////////////////////////////////////////////////////////////////

void ControlInstance::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    click_pos = event->pos();
    control->beginDrag();
    QGraphicsObject::mousePressEvent(event);
}

void ControlInstance::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    control->endDrag();
    QGraphicsObject::mouseReleaseEvent(event);
}

void ControlInstance::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsObject::mouseMoveEvent(event);

    QMatrix4x4 mi = getMatrix().inverted();
    QVector3D p0 = mi * QVector3D(click_pos);
    QVector3D p1 = mi * QVector3D(event->pos());

    QVector3D eye = (mi*QVector3D(0, 0, -1)).normalized();

    control->drag(p1 + eye * QVector3D::dotProduct(eye, control->pos() - p1),
                  p1 - p0);

    click_pos = event->pos();
}

void ControlInstance::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsObject::hoverEnterEvent(event);
    if (!hover)
    {
        hover = true;
        update();
    }
}

void ControlInstance::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsObject::hoverLeaveEvent(event);
    if (hover)
    {
        hover = false;
        update();
    }
}

void ControlInstance::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete ||
        event->key() == Qt::Key_Backspace)
    {
        control->deleteNode();
    }
    else
    {
        event->ignore();
    }
}

void ControlInstance::redraw()
{
    prepareGeometryChange();
}
