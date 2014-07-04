#include <Python.h>

#include <QGraphicsSceneMouseEvent>

#include "ui/connection.h"
#include "ui/canvas.h"
#include "ui/colors.h"

#include "datum/datum.h"
#include "datum/link.h"

#include "node/node.h"

#include "control/control.h"

Connection::Connection(Link* link, Canvas* canvas)
    : QGraphicsObject(), link(link), canvas(canvas)
{
    setFlags(QGraphicsItem::ItemIsSelectable);
    canvas->scene->addItem(this);
    setZValue(2);
    connect(startControl(), &Control::portPositionChanged,
            this, &Connection::onPortPositionChanged);
}

QRectF Connection::boundingRect() const
{
    return path().boundingRect();
}

QPainterPath Connection::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(4);
    return stroker.createStroke(path());
}

Datum* Connection::startDatum() const
{
    return dynamic_cast<Datum*>(link->parent());
}

Control* Connection::startControl() const
{
    return canvas->getControl(dynamic_cast<Node*>(startDatum()->parent()));
}

Datum* Connection::endDatum() const
{
    return link->target ? link->target : NULL;
}

Control* Connection::endControl() const
{
    if (endDatum() == NULL)
    {
        return NULL;
    }
    return canvas->getControl(dynamic_cast<Node*>(endDatum()->parent()));
}

QPointF Connection::startPos() const
{
    return startControl()->datumOutputPosition(startDatum());
}

QPointF Connection::endPos() const
{
    Control* c = endControl();
    return c ? c->datumInputPosition(endDatum()) : drag_pos;
}

QPainterPath Connection::path() const
{
    QPointF start = startPos();
    QPointF end = endPos();

    float length = 50;
    if (end.x() <= start.x())
    {
        length += (start.x() - end.x()) / 2;
    }

    QPainterPath p;
    p.moveTo(start);
    p.cubicTo(start + QPointF(length, 0),
              end - QPointF(length, 0), end);
   return p;
}

void Connection::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QColor color = Colors::getColor(dynamic_cast<Datum*>(link->parent()));
    if (isSelected())
    {
        color = Colors::highlight(color);
    }

    painter->setPen(QPen(color, 4));
    painter->drawPath(path());
}

void Connection::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (link->target)
    {
        return;
    }

    prepareGeometryChange();
    drag_pos = event->pos();
}

void Connection::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseReleaseEvent(event);
    ungrabMouse();
}
