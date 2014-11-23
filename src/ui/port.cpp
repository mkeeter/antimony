#include <Python.h>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>
#include <QToolTip>

#include "ui/port.h"
#include "ui/inspector/inspector.h"
#include "ui/colors.h"
#include "ui/connection.h"
#include "ui/canvas.h"
#include "ui/tooltip.h"

#include "graph/datum/datum.h"

Port::Port(Datum* d, Canvas* canvas, QGraphicsItem* parent)
    : QGraphicsObject(parent), datum(d), canvas(canvas), hover(false)
{
    setAcceptHoverEvents(true);
}

QRectF Port::boundingRect() const
{
    return QRectF(0, 0, 10, 10);
}

void Port::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (datum.isNull())
    {
        return;
    }

    QColor color = Colors::getColor(datum);
    if (hover)
    {
        color = Colors::highlight(color);
    }
    painter->setBrush(color);

    painter->setPen(Qt::NoPen);
    painter->drawRect(boundingRect());
}

Datum* Port::getDatum() const
{
    return datum;
}

////////////////////////////////////////////////////////////////////////////////

InputPort::InputPort(Datum *d, Canvas *canvas, QGraphicsItem *parent)
    : Port(d, canvas, parent)
{
    // Nothing to do here
}

////////////////////////////////////////////////////////////////////////////////

OutputPort::OutputPort(Datum *d, Canvas *canvas, QGraphicsItem *parent)
    : Port(d, canvas, parent)
{
    // Nothing to do here
}

void OutputPort::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        Link* link = datum->linkFrom();
        Connection* c = new Connection(link, canvas);
        c->setDragPos(mapToScene(event->pos()));
        c->grabMouse();
    }
    else
    {
        event->ignore();
    }
}
void OutputPort::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    hover = true;
    QGraphicsItem::hoverEnterEvent(event);
}

void OutputPort::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    hover = false;
    QGraphicsItem::hoverLeaveEvent(event);
}
