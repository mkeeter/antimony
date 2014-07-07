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

    connect(link, SIGNAL(destroyed()), this, SLOT(deleteLater()),
            Qt::QueuedConnection);
    connect(this, SIGNAL(destroyed()), canvas, SLOT(update()));
}

QRectF Connection::boundingRect() const
{
    return areDatumsValid() ? path().boundingRect() : QRectF();
}

QPainterPath Connection::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(4);
    return stroker.createStroke(path());
}

bool Connection::areDatumsValid() const
{
    return link && dynamic_cast<Datum*>(link->parent()) &&
            (drag_state != CONNECTED || link->target);
}

Datum* Connection::startDatum() const
{
    Datum* d = dynamic_cast<Datum*>(link->parent());
    Q_ASSERT(d);
    return d;
}

Datum* Connection::endDatum() const
{
    Q_ASSERT(drag_state == CONNECTED);
    Q_ASSERT(link->target);
    return link->target;
}

Node* Connection::startNode() const
{
    Node* n = dynamic_cast<Node*>(startDatum()->parent());
    Q_ASSERT(n);
    return n;
}

Node* Connection::endNode() const
{
    Q_ASSERT(drag_state == CONNECTED);
    Node* n = dynamic_cast<Node*>(endDatum()->parent());
    Q_ASSERT(n);
    return n;
}

Control* Connection::startControl() const
{
    Control* c = canvas->getControl(startNode());
    Q_ASSERT(c);
    return c;
}


Control* Connection::endControl() const
{
    Q_ASSERT(drag_state == CONNECTED);
    Control* c = canvas->getControl(endNode());
    Q_ASSERT(c);
    return c;
}

QPointF Connection::startPos() const
{
    Q_ASSERT(startControl());
    return startControl()->datumOutputPosition(startDatum());
}

QPointF Connection::endPos() const
{
    if (drag_state == CONNECTED)
    {
        return endControl()->datumInputPosition(endDatum());
    }
    else
    {
        return drag_pos;
    }
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
    if (link.isNull())
    {
        return;
    }

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
    if (drag_state == CONNECTED)
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
    if (drag_state == CONNECTED)
    {
        return;
    }

    ungrabMouse();

    InputPort* target = canvas->getInputPortAt(event->pos());
    Datum* datum = target ? target->getDatum() : NULL;
    if (target && datum->acceptsLink(link))
    {
        datum->addLink(link);
        drag_state = CONNECTED;

        connect(endControl(), &Control::portPositionChanged,
                this, &Connection::onPortPositionChanged);
    }
    else
    {
        link->deleteLater();
    }

    prepareGeometryChange();
}
