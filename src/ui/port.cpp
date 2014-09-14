#include <Python.h>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>
#include <QToolTip>

#include "ui/port.h"
#include "ui/inspector/inspector.h"
#include "ui/colors.h"
#include "ui/connection.h"

#include "datum/datum.h"

Port::Port(Datum* d, Canvas* canvas, QGraphicsItem* parent) :
    QGraphicsObject(parent), datum(d), canvas(canvas), _opacity(1), hover(false)
{
    setAcceptHoverEvents(true);
}

QRectF Port::boundingRect() const
{
    return QRectF(0, 0, 10, 10);
}

void Port::setOpacity(float o)
{
    _opacity = o;
    if (o == 0) hide();
    else        show();
    update();
}

void Port::fadeIn()
{
    QPropertyAnimation* a = new QPropertyAnimation(this, "opacity", this);
    a->setDuration(100);
    a->setStartValue(0);
    a->setEndValue(1);
    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void Port::fadeOut()
{
    QPropertyAnimation* a = new QPropertyAnimation(this, "opacity", this);
    a->setDuration(100);
    a->setStartValue(1);
    a->setEndValue(0);
    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void Port::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget)
{
    if (datum.isNull())
    {
        return;
    }

    Q_UNUSED(option);
    Q_UNUSED(widget);

    QColor color = Colors::getColor(datum);
    if (hover)
    {
        color = Colors::highlight(color);
    }
    color.setAlpha(_opacity*255);
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

OutputPort::OutputPort(Datum *d, Canvas *canvas, QGraphicsItem *parent)
    : Port(d, canvas, parent)
{
    // Nothing to do here
}

void OutputPort::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    hover = true;
    update();
}

void OutputPort::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    hover = false;
    update();
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
