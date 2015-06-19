#include <Python.h>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>
#include <QToolTip>

#include "app/app.h"

#include "ui/canvas/port.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/connection.h"

#include "ui/util/colors.h"

#include "graph/datum/datum.h"

Port::Port(Datum* d, QGraphicsItem* parent)
    : QGraphicsObject(parent), datum(d), hover(false)
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
        return;

    QColor color = Colors::getColor(datum);
    if (hover)
        color = Colors::highlight(color);
    painter->setBrush(color);

    painter->setPen(Qt::NoPen);
    painter->drawRect(boundingRect());
}

Datum* Port::getDatum() const
{
    return datum;
}

////////////////////////////////////////////////////////////////////////////////

InputPort::InputPort(Datum *d, QGraphicsItem *parent)
    : Port(d, parent)
{
    // Nothing to do here
}

////////////////////////////////////////////////////////////////////////////////

OutputPort::OutputPort(Datum *d, QGraphicsItem *parent)
    : Port(d, parent)
{
    // Nothing to do here
}

void OutputPort::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        Connection* c = App::instance()->newLink(datum->linkFrom());
        c->setDragPos(mapToScene(event->pos()));
        c->grabMouse();
        c->setFocus();

        // Turn off the hover highlighting.
        hover = false;
        update();
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
