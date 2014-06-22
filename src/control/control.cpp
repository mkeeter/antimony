#include <Python.h>

#include <algorithm>

#include <QGraphicsSceneMouseEvent>

#include "control/control.h"
#include "node/node.h"
#include "ui/canvas.h"

Control::Control(Canvas* canvas, Node* node, QGraphicsItem* parent)
    : QGraphicsObject(parent), canvas(canvas), node(node), viewer(NULL),
      _hover(false), _dragged(false)
{
    setFlags(QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemIgnoresTransformations);
    setAcceptHoverEvents(true);

    if (parent == NULL)
    {
        canvas->scene->addItem(this);
    }
}

QRectF Control::boundingBox(QVector<QVector3D> points, int padding) const
{
    float xmin =  INFINITY;
    float xmax = -INFINITY;
    float ymin =  INFINITY;
    float ymax = -INFINITY;

    for (auto p : points)
    {
        QPointF o = canvas->worldToScene(p);
        if (o.x() < xmin)   xmin = o.x();
        if (o.x() > xmax)   xmax = o.x();
        if (o.y() < ymin)   ymin = o.y();
        if (o.y() > ymax)   ymax = o.y();
    }

    return QRectF(xmin - padding, ymin - padding,
                  xmax - xmin + 2*padding,
                  ymax - ymin + 2*padding);
}

void Control::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (!_hover)
    {
        _hover = true;
        update();
    }
}

void Control::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (_hover)
    {
        _hover = false;
        update();
    }
}

void Control::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (parentObject())
    {
        dynamic_cast<Control*>(parentObject())->mouseDoubleClickEvent(event);
    }
    else if (viewer == NULL)
    {
        // Open up a node viewer here.
    }
}

void Control::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    _dragged = false;
    _click_pos = event->pos();
}

void Control::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!_dragged && event->button() == Qt::LeftButton)
    {
        setSelected(true);
    }
    ungrabMouse();
}

void Control::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QVector3D p0 = canvas->sceneToWorld(_click_pos);
    QVector3D p1 = canvas->sceneToWorld(event->pos());

    drag(p1, p1 - p0);

    _click_pos = event->pos();
    _dragged = true;
}

////////////////////////////////////////////////////////////////////////////////

DummyControl::DummyControl(Canvas *canvas, Node *node, QGraphicsItem *parent)
    : Control(canvas, node, parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setAcceptHoverEvents(false);
    setZValue(-10);
}

void DummyControl::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(center);
    Q_UNUSED(delta);

    // A DummyControl should never be dragged.
    Q_ASSERT(false);
}
