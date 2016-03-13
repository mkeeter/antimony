#include <Python.h>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "app/colors.h"
#include "canvas/scene.h"
#include "canvas/datum_port.h"
#include "canvas/datum_row.h"
#include "canvas/connection/dummy.h"

DatumPort::DatumPort(Datum* d, DatumRow* parent)
    : QGraphicsObject(parent), datum(d), hover(false),
      color(Colors::getColor(d))
{
    setAcceptHoverEvents(true);
    setFlags(QGraphicsItem::ItemSendsScenePositionChanges);
}

QVariant DatumPort::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemScenePositionHasChanged)
        emit(moved());
    else if (change == ItemVisibleHasChanged)
        emit(hiddenChanged());
    return value;
}

QRectF DatumPort::boundingRect() const
{
    return QRectF(0, 0, 20, 20);
}

void DatumPort::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(hover ? Colors::highlight(color) : color);
    painter->setPen(Qt::NoPen);
    painter->drawRect(portRect());
}

////////////////////////////////////////////////////////////////////////////////

InputPort::InputPort(Datum* d, DatumRow* parent)
    : DatumPort(d, parent)
{
    // Nothing to do here
}

QRectF InputPort::portRect() const
{
    return QRectF(0, 5, 10, 10);
}

////////////////////////////////////////////////////////////////////////////////

OutputPort::OutputPort(Datum* d, DatumRow* parent)
    : DatumPort(d, parent)
{
    // Nothing to do here
}

QRectF OutputPort::portRect() const
{
    return QRectF(10, 5, 10, 10);
}

void OutputPort::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        auto cs = static_cast<CanvasScene*>(scene());
        auto conn = new DummyConnection(this, cs);
        conn->setDragPos(mapToScene(event->pos()));
        conn->grabMouse();
        conn->setFocus();

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
