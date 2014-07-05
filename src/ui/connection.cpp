#include <Python.h>

#include <QGraphicsSceneMouseEvent>

#include "ui/connection.h"
#include "ui/canvas.h"
#include "ui/colors.h"
#include "ui/port.h"

#include "datum/datum.h"
#include "datum/link.h"

#include "node/node.h"

#include "control/control.h"

Connection::Connection(Link* link, Canvas* canvas)
    : QGraphicsObject(), link(link), canvas(canvas), drag_state(NONE)
{
    setFlags(QGraphicsItem::ItemIsSelectable);
    canvas->scene->addItem(this);
    setZValue(2);
    connect(startControl(), &Control::portPositionChanged,
            this, &Connection::onPortPositionChanged);
    connect(link, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed()), canvas, SLOT(update()));
}

QRectF Connection::boundingRect() const
{
    return link ? path().boundingRect() : QRectF();
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

    QColor color = Colors::getColor(startDatum());
    if (drag_state == INVALID)
    {
        color = Colors::red;
    }
    if (isSelected() || drag_state == VALID)
    {
        color = Colors::highlight(color);
    }

    painter->setPen(QPen(color, 4));
    painter->drawPath(path());
}

void Connection::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!link || link->target)
    {
        return;
    }
    InputPort* target = canvas->getInputPortAt(event->pos());
    if (target && target->getDatum()->acceptsLink(link))
    {
        drag_state = VALID;
    }
    else if (target)
    {
        drag_state = INVALID;
    }
    else
    {
        drag_state = NONE;
    }
    prepareGeometryChange();
    drag_pos = event->pos();
}

void Connection::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseReleaseEvent(event);
    ungrabMouse();

    if (!link || link->target)
    {
        return;
    }

    InputPort* target = canvas->getInputPortAt(event->pos());
    Datum* datum = target ? target->getDatum() : NULL;
    if (target && datum->acceptsLink(link))
    {
        datum->addLink(link);
        connect(endControl(), &Control::portPositionChanged,
                this, &Connection::onPortPositionChanged);

        // Making this connection could cause ports to move around
        prepareGeometryChange();

        drag_state = NONE;
    }
    else
    {
        link->deleteLater();
    }
}
