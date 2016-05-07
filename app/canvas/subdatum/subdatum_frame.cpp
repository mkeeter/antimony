#include <Python.h>

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include "canvas/subdatum/subdatum_frame.h"
#include "canvas/subdatum/subdatum_row.h"
#include "canvas/scene.h"

#include "app/colors.h"

const float SubdatumFrame::PADDING = 3;

////////////////////////////////////////////////////////////////////////////////

SubdatumFrame::SubdatumFrame(Datum* d, QGraphicsScene* scene)
    : datum(d), datum_row(new SubdatumRow(d, this))
{
    setFlags(QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    scene->addItem(this);
    datum_row->padLabel(datum_row->labelWidth());
    redoLayout();
}

////////////////////////////////////////////////////////////////////////////////

QRectF SubdatumFrame::boundingRect() const
{
    QRectF b;
    for (auto c : childItems())
        if (c->isVisible())
            b = b.united(c->boundingRect().translated(c->pos()));
    b.setBottom(b.bottom() + PADDING);
    b.setTop(b.top() - PADDING);
    return b;
}

////////////////////////////////////////////////////////////////////////////////

void SubdatumFrame::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    const auto r = boundingRect();

    // Draw interior
    painter->setBrush(Colors::base01);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(r, 8, 8);

    // Draw outer edge
    painter->setBrush(Qt::NoBrush);
    if (isSelected())
        painter->setPen(QPen(Colors::base05, 2));
    else
        painter->setPen(QPen(Colors::base03, 2));
    painter->drawRoundedRect(r, 8, 8);
}

////////////////////////////////////////////////////////////////////////////////

void SubdatumFrame::redoLayout()
{
    datum_row->setWidth(datum_row->minWidth());
    prepareGeometryChange();
}

////////////////////////////////////////////////////////////////////////////////

void SubdatumFrame::update(const DatumState& state)
{
    datum_row->update(state);
}

////////////////////////////////////////////////////////////////////////////////

void SubdatumFrame::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (dragging)
    {
        setPos(event->scenePos());
        event->accept();
    }
    else
    {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void SubdatumFrame::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);

    if (dragging)
    {
        ungrabMouse();
    }
    else if (event->button() == Qt::LeftButton)
    {
        // Store an Undo command for this drag
        const auto delta = event->scenePos() -
                     event->buttonDownScenePos(Qt::LeftButton);
        static_cast<CanvasScene*>(scene())->endDrag(delta);
    }
    dragging = false;
}
