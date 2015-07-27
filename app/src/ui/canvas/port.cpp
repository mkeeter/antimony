#include <Python.h>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>
#include <QToolTip>

#include "app/app.h"

#include "ui/canvas/port.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/connection.h"
#include "ui/canvas/graph_scene.h"

#include "ui/util/colors.h"

#include "graph/datum.h"

Port::Port(Datum* d, QGraphicsItem* parent)
    : QGraphicsObject(parent), datum(d), hover(false),
      color(Colors::getColor(d))
{
    setAcceptHoverEvents(true);
    setFlags(QGraphicsItem::ItemSendsScenePositionChanges);
}

QVariant Port::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemScenePositionHasChanged)
        emit(moved());
    else if (change == ItemVisibleHasChanged)
        emit(hiddenChanged());
    return value;
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

    painter->setBrush(hover ? Colors::highlight(color) : color);
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
    d->installWatcher(this);
    trigger(d->getState());
}

InputPort::~InputPort()
{
    for (auto c : connections)
        c->deleteLater();
}

void InputPort::trigger(const DatumState& state)
{
    auto itr=connections.begin();
    while (itr != connections.end())
        if (state.links.count(itr.key()) == 0)
        {
            itr.value()->deleteLater();
            itr = connections.erase(itr);
        }
        else
        {
            itr++;
        }

    for (auto d : state.links)
        if (!connections.contains(d))
            static_cast<GraphScene*>(scene())->makeLink(d, this);
}

void InputPort::install(Connection* c)
{
    connect(this, &Port::moved,
            c, &Connection::onPortsMoved);
    connect(this, &Port::hiddenChanged,
            c, &Connection::onHiddenChanged);

    Q_ASSERT(!connections.contains(c->source->getDatum()));
    connections[c->source->getDatum()] = c;
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
        auto g = static_cast<GraphScene*>(scene());
        Connection* c = g->makeLinkFrom(datum);
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
