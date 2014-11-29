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

void ControlProxy::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsObject::mouseMoveEvent(event);

    QVector3D p0 = sceneToWorld(click_pos);
    QVector3D p1 = sceneToWorld(event->pos());

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

QVector3D ControlProxy::sceneToWorld(QPointF p) const
{
    QMatrix4x4 m = getMatrix().inverted();
    return m * QVector3D(p);
}

QPointF ControlProxy::worldToScene(QVector3D v) const
{
    return (getMatrix() * v).toPointF();
}


QRectF ControlProxy::boundingBox(QVector<QVector3D> points, int padding) const
{
    float xmin =  INFINITY;
    float xmax = -INFINITY;
    float ymin =  INFINITY;
    float ymax = -INFINITY;

    for (auto p : points)
    {
        QPointF o = worldToScene(p);
        if (o.x() < xmin)   xmin = o.x();
        if (o.x() > xmax)   xmax = o.x();
        if (o.y() < ymin)   ymin = o.y();
        if (o.y() > ymax)   ymax = o.y();
    }

    return QRectF(xmin - padding, ymin - padding,
                  xmax - xmin + 2*padding,
                  ymax - ymin + 2*padding);
}

QMatrix4x4 ControlProxy::getMatrix() const
{
    return viewport->getMatrix();
}
