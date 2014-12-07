#include <Python.h>

#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

#include "control/proxy.h"
#include "control/control.h"
#include "ui/viewport/viewport.h"

ControlProxy::ControlProxy(Control* control, Viewport* viewport)
    : QGraphicsObject(), control(control), viewport(viewport), hover(false)
{
    setFlags(QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(true);

    connect(control, &Control::destroyed,
            this, &ControlProxy::deleteLater);
    connect(control, &Control::redraw,
            this, &ControlProxy::redraw);
}

void ControlProxy::redraw()
{
    prepareGeometryChange();
}

QRectF ControlProxy::boundingRect() const
{
    return control->bounds(getMatrix());
}

QPainterPath ControlProxy::shape() const
{
    return control->shape(getMatrix());
}

void ControlProxy::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem* option,
                         QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    control->paint(getMatrix(), isSelected() || hover, painter);
}

void ControlProxy::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (!hover)
    {
        hover = true;
        update();
    }
}

void ControlProxy::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (hover)
    {
        hover = false;
        update();
    }
}

void ControlProxy::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsObject::mousePressEvent(event);
    click_pos = event->pos();
}

void ControlProxy::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
    if (scene()->mouseGrabberItem() == this)
        ungrabMouse();
}

void ControlProxy::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsObject::mouseMoveEvent(event);

    QMatrix4x4 mi = getMatrix().inverted();
    QVector3D p0 = mi * QVector3D(click_pos);
    QVector3D p1 = mi * QVector3D(event->pos());

    control->drag(p1, p1 - p0);
    click_pos = event->pos();
}

void ControlProxy::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete ||
        event->key() == Qt::Key_Backspace)
        control->deleteNode();
    else
        event->ignore();
}

QMatrix4x4 ControlProxy::getMatrix() const
{
    return viewport->getMatrix();
}


Node* ControlProxy::getNode() const
{
    return control ? control->getNode() : NULL;
}

Control* ControlProxy::getControl() const
{
    return control;
}
