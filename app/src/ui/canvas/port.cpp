#include <Python.h>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>
#include <QToolTip>

#include "app/app.h"

#include "ui/canvas/port.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/inspector/inspector_row.h"
#include "ui/canvas/inspector/inspector_text.h"
#include "ui/canvas/connection.h"
#include "ui/canvas/graph_scene.h"

#include "ui/util/colors.h"

#include "graph/datum.h"


const qreal Port::Width = 15;
const qreal Port::PaintSize = 10;

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

void Port::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget)
{
    // the painted port rectangle

    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(hover ? Colors::highlight(color) : color);
    painter->setPen(Qt::NoPen);
    painter->drawRect(paintRect());
}

QRectF Port::paintRect() const
{
    const int editor_h = (dynamic_cast<InspectorRow*>(this->parentItem()))->editor->boundingRect().height();
    const qreal dx = adjust_left_right();
    const qreal dy = (editor_h - PaintSize)/2;
    return QRectF(
        dx, dy, // from
        PaintSize, PaintSize); // width,height
}

qreal Port::adjust_left_right() const 
{
    return InspectorRow::LeftPadding;
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

QRectF InputPort::boundingRect() const
{
    // we are lying a bit here:
    // boundingRect is used for drag/drop detection, and we want to act like we are bigger for convenience
    // See dropRect() for where the connector attaches
    auto editor = (dynamic_cast<InspectorRow*>(this->parentItem()))->editor;
    int h = editor->boundingRect().height();
    // us + label + right-padding + editor
    int w = Port::Width + (dynamic_cast<InspectorRow*>(this->parentItem()))->labelWidth() + InspectorRow::LabelPadding + editor->boundingRect().width() - 1; // -1 for slightly better disambiguation?
    return QRectF(0, 0, w, h);
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

QRectF OutputPort::boundingRect() const
{
    // we are lying a bit here:
    // boundingRect is used for drag/drop detection, and we want to act like we are bigger for convenience
    // See dropRect() for where the connector attaches
    auto editor = (dynamic_cast<InspectorRow*>(this->parentItem()))->editor;
    int h = editor->boundingRect().height();
    // left-padding + paint + right-padding
    return QRectF(-InspectorRow::TextPadding + 1 + 4, 0, Width+8, h); // FIXME: text-box "margin" + inspecter "8"
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

qreal OutputPort::adjust_left_right() const
{
    // right_justify + 8pixels-of-title-row - 2gap
    return Width - PaintSize + NodeInspector::TitleRightPadding - 3;
}

