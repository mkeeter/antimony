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

#include "datum/datum.h"

Port::Port(Datum* d, Canvas* canvas, QGraphicsItem* parent)
    : QGraphicsObject(parent), datum(d), canvas(canvas),
      label(new ToolTipItem(datum->objectName())),
      _opacity(parent ? true : canvas->arePortsVisible()),
      hover(false), visible(parent ? true : false),
      visible_g(parent ? true : canvas->arePortsVisible())
{
    setAcceptHoverEvents(true);
    canvas->scene->addItem(label);
    label->hide();

    // Free-floating ports can have visibility toggled.
    if (!parent)
    {
        connect(canvas, &Canvas::showPorts,
                this, &Port::setVisibleG);
    }

    if (!(visible && visible_g))
    {
        hide();
    }
}

Port::~Port()
{
    // As the label isn't a child widget, we have to delete it.
    if (label)
        label->deleteLater();
}

QRectF Port::boundingRect() const
{
    return QRectF(0, 0, 10, 10);
}

void Port::setVisible(bool v)
{
    bool was_visible = visible && visible_g;
    visible = v;
    bool now_visible = visible && visible_g;
    if (now_visible != was_visible)
    {
        if (now_visible)
            fadeIn();
        else
            fadeOut();
    }
}

void Port::setVisibleG(bool v)
{
    bool was_visible = visible && visible_g;
    visible_g = v;
    bool now_visible = visible && visible_g;
    if (now_visible != was_visible)
    {
        if (now_visible)
            fadeIn();
        else
            fadeOut();
    }
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
    show();
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

void Port::setPos(QPointF pos)
{
    QGraphicsItem::setPos(pos);
    QPointF p = mapToScene(boundingRect().center());
    label->setPos(p + label_offset);
}

void Port::showToolTip()
{
    // Only show tooltips for free-floating ports.
    if (!parentItem())
        label->show();
}

void Port::hideToolTip()
{
    label->hide();
}

////////////////////////////////////////////////////////////////////////////////

InputPort::InputPort(Datum *d, Canvas *canvas, QGraphicsItem *parent)
    : Port(d, canvas, parent)
{
    label_offset = QPointF(-12 - label->boundingRect().width(), -12);
    label->setPos(-2 - label->boundingRect().width(), -12);
}

void InputPort::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    showToolTip();
}

void InputPort::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    hideToolTip();
}

////////////////////////////////////////////////////////////////////////////////

OutputPort::OutputPort(Datum *d, Canvas *canvas, QGraphicsItem *parent)
    : Port(d, canvas, parent)
{
    label_offset = QPointF(12, -12);
}

void OutputPort::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    showToolTip();
    hover = true;
    update();
}

void OutputPort::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    hideToolTip();
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
