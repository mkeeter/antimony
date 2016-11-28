#include <Python.h>

#include <QGraphicsScene>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>

#include "canvas/inspector/frame.h"
#include "canvas/datum_row.h"
#include "canvas/datum_editor.h"
#include "canvas/inspector/title.h"
#include "canvas/inspector/export.h"
#include "canvas/scene.h"

#include "graph/node.h"

#include "app/colors.h"

const float InspectorFrame::PADDING_ROWS = 3;

InspectorFrame::InspectorFrame(Node* node, QGraphicsScene* scene)
    : QGraphicsObject(), node(node), title_row(new InspectorTitle(node, this)),
      export_button(new InspectorExportButton(this)),
      show_hidden(false), dragging(false)
{
    setFlags(QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    scene->addItem(this);
    redoLayout();
}

QRectF InspectorFrame::tightBoundingRect() const
{
    QRectF b;
    for (auto c : childItems())
    {
        if (c->isVisible())
        {
            b = b.united(c->boundingRect().translated(c->pos()));
        }
    }
    b.setBottom(b.bottom() + PADDING_ROWS);
    return b;
}

QList<DatumRow*> InspectorFrame::visibleRows() const
{
    QList<DatumRow*> rows;
    for (auto c : childItems())
        if (auto row = dynamic_cast<DatumRow*>(c))
        {
            if (show_hidden || !row->shouldBeHidden())
            {
                rows.append(row);
            }
        }

    // Sort datums by row order
    qSort(rows.begin(), rows.end(),
          [](const DatumRow* a, const DatumRow* b)
          { return a->getIndex() < b->getIndex(); });

    return rows;
}

QRectF InspectorFrame::boundingRect() const
{
    auto r = tightBoundingRect();
    r += {10, 10, 10, 10};
    return r;
}

void InspectorFrame::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    const auto r = tightBoundingRect();

    // Draw higlight
    if (has_focus)
    {
        painter->setPen(QPen(QColor(255, 255, 255, 128), 10));
        painter->drawRoundedRect(r, 8, 8);
    }

    // Draw interior
    painter->setBrush(Colors::base01);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(r, 8, 8);

    {   // Draw light-colored rectangle under title bar
        painter->setBrush(Colors::base03);
        painter->drawRoundedRect(title_row->boundingRect(), 8, 8);
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(Colors::base03, 4));
        const auto y = title_row->boundingRect().bottom();
        painter->drawLine(2, y, tightBoundingRect().right() - 2, y);
    }

    // Draw outer edge
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(isSelected() ? Colors::base05 : Colors::base03, 2));
    painter->drawRoundedRect(r, 8, 8);
}

////////////////////////////////////////////////////////////////////////////////

void InspectorFrame::setNameValid(bool valid)
{
    title_row->setNameValid(valid);
}

////////////////////////////////////////////////////////////////////////////////

void InspectorFrame::setTitle(QString title)
{
    title_row->setTitle(title);
}

////////////////////////////////////////////////////////////////////////////////

void InspectorFrame::setShowHidden(bool h)
{
    if (h != show_hidden)
    {
        show_hidden = h;
        redoLayout();
    }
}

////////////////////////////////////////////////////////////////////////////////

void InspectorFrame::redoLayout()
{
    QList<DatumRow*> rows;
    for (auto c : childItems())
        if (auto row = dynamic_cast<DatumRow*>(c))
        {
            if (show_hidden || !row->shouldBeHidden())
            {
                rows.append(row);
            }
            else
            {
                row->hide();
            }
        }

    // Show all rows that made it into our list
    for (auto r : rows)
    {
        r->show();
    }

    // Sort datums by row order
    qSort(rows.begin(), rows.end(),
          [](const DatumRow* a, const DatumRow* b)
          { return a->getIndex() < b->getIndex(); });

    {   // Pad the row labels for alignment
        float max_label = 0;
        for (auto row : rows)
            max_label = std::max(max_label, row->labelWidth());
        for (auto row : rows)
            row->padLabel(max_label);
    }

    {   // Pad all of the rows (including the title) to the same width
        float max_width = title_row->minWidth();
        for (auto row : rows)
            max_width = std::max(max_width, row->minWidth());
        title_row->setWidth(max_width);
        for (auto row : rows)
            row->setWidth(max_width);
    }

    {   // Spread out the rows along the Y axis
        float y = title_row->boundingRect().height() + PADDING_ROWS;
        for (auto row : rows)
        {
            row->setPos(0, y);
            y += row->boundingRect().height() + PADDING_ROWS;
        }

        // Set the export button's size and position
        if (export_button->isVisible()) {
            auto w = boundingRect().width() / 2;
            export_button->setPos(w/2, y);
            export_button->setWidth(w);
        }
    }

    prepareGeometryChange();
}

////////////////////////////////////////////////////////////////////////////////

void InspectorFrame::setFocus(bool focus)
{
    if (focus != has_focus)
    {
        has_focus = focus;
        prepareGeometryChange();
    }
}

void InspectorFrame::focusNext(DatumEditor* prev)
{
    bool next = false;

    for (auto row : visibleRows())
    {
        if (prev == row->editor)
        {
            next = true;
        }
        else if (next && row->editor->isEnabled())
        {
            prev->clearFocus();
            row->editor->setFocus();
            return;
        }
    }
}

void InspectorFrame::focusPrev(DatumEditor* next)
{
    DatumRow* prev = NULL;

    for (auto row : visibleRows())
    {
        if (next == row->editor)
        {
            if (prev)
            {
                prev->editor->setFocus();
                next->clearFocus();
            }
            return;
        }
        prev = row;
    }
}

////////////////////////////////////////////////////////////////////////////////

void InspectorFrame::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
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

void InspectorFrame::contextMenuEvent(QGraphicsSceneContextMenuEvent* e)
{
    Q_UNUSED(e);
    QString desc = QString::fromStdString(node->getName());

    QScopedPointer<QMenu> menu(new QMenu());
    auto jump_to = new QAction("Zoom to " + desc, menu.data());

    menu->addAction(jump_to);
    connect(jump_to, &QAction::triggered, this, &InspectorFrame::onZoomTo);

    menu->exec(QCursor::pos());
}

void InspectorFrame::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
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
        if (delta != QPointF(0,0))
        {
            static_cast<CanvasScene*>(scene())->endDrag(delta);
        }
    }
    dragging = false;
}

////////////////////////////////////////////////////////////////////////////////

void InspectorFrame::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverEnterEvent(event);
    emit(onFocus(true));
}

void InspectorFrame::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverLeaveEvent(event);
    emit(onFocus(false));
}

////////////////////////////////////////////////////////////////////////////////

void InspectorFrame::setExportWorker(ExportWorker* worker)
{
    if (export_button->setWorker(worker))
    {
        redoLayout();
    }
}

void InspectorFrame::clearExportWorker()
{
    if (export_button->clearWorker())
    {
        redoLayout();
    }
}
