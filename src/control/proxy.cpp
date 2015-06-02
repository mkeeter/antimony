#include <Python.h>

#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

#include "control/proxy.h"
#include "control/control.h"
#include "ui/viewport/viewport.h"

ControlProxy::ControlProxy(Control* control, Viewport* viewport)
    : control(control), viewport(viewport), hover(false),
      changing_selection(false)
{
    setFlags(QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemIsFocusable);
    setAcceptHoverEvents(true);

    connect(control, &Control::destroyed,
            this, &ControlProxy::deleteLater);
    connect(viewport, &Viewport::destroyed,
            this, &ControlProxy::deleteLater);

    connect(control, &Control::redraw,
            this, &ControlProxy::redraw);
    connect(viewport, &Viewport::viewChanged,
            this, &ControlProxy::redraw);

    connect(control, &Control::changeProxySelection,
            this, &ControlProxy::selectProxy);

    viewport->scene->addItem(this);

    if (viewport->isUIhidden())
        hide();
}

void ControlProxy::redraw()
{
    prepareGeometryChange();
}

void ControlProxy::selectProxy(bool s)
{
    changing_selection = true;
    setSelected(s);
    changing_selection = false;
}

QRectF ControlProxy::boundingRect() const
{
    return (control && control->getNode())
        ? control->bounds(getMatrix())
        : QRectF();
}

QPainterPath ControlProxy::shape() const
{
    return (control && control->getNode())
        ? control->shape(getMatrix())
        : QPainterPath();
}

void ControlProxy::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem* option,
                         QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (control && control->getNode())
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
    emit(control->glowChanged(control->getNode(), true));
}

void ControlProxy::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (hover)
    {
        hover = false;
        update();
    }
    emit(control->glowChanged(control->getNode(), false));
}

void ControlProxy::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    click_pos = event->pos();
    control->beginDrag();
    QGraphicsObject::mousePressEvent(event);
}

void ControlProxy::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    control->endDrag();
    QGraphicsObject::mouseReleaseEvent(event);
}

void ControlProxy::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
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

QVariant ControlProxy::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (!changing_selection && change == QGraphicsItem::ItemSelectedChange)
    {
        changing_selection = true;
        emit(control->proxySelectionChanged(value.toBool()));
        changing_selection = false;
    }


    return QGraphicsItem::itemChange(change, value);
}
