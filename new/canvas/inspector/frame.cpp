#include <Python.h>

#include <QGraphicsScene>
#include <QPainter>

#include "canvas/inspector/frame.h"
#include "canvas/inspector/row.h"
#include "canvas/inspector/title.h"
#include "app/colors.h"

const float InspectorFrame::PADDING_ROWS = 3;

InspectorFrame::InspectorFrame(Node* node, QGraphicsScene* scene)
    : QGraphicsObject(), title_row(new InspectorTitle(node, this))
{
    setFlags(QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    Q_UNUSED(node);

    scene->addItem(this);
    redoLayout();
}

QRectF InspectorFrame::boundingRect() const
{
    auto r = childrenBoundingRect();
    r.setBottom(r.bottom() + PADDING_ROWS);
    return r;
}

void InspectorFrame::paint(QPainter *painter,
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

    {   // Draw light-colored rectangle under title bar
        painter->setBrush(Colors::base03);
        painter->drawRoundedRect(title_row->boundingRect(), 8, 8);
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(Colors::base03, 4));
        const auto y = title_row->boundingRect().bottom();
        painter->drawLine(2, y, boundingRect().right() - 2, y);
    }

    // Draw outer edge
    painter->setBrush(Qt::NoBrush);
    if (isSelected())
        painter->setPen(QPen(Colors::base05, 2));
    else
        painter->setPen(QPen(Colors::base03, 2));
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

void InspectorFrame::redoLayout()
{
    QList<InspectorRow*> rows;
    for (auto c : childItems())
        if (auto row = dynamic_cast<InspectorRow*>(c))
            rows.append(row);

    // Sort datums by row order
    qSort(rows.begin(), rows.end(),
          [](const InspectorRow* a, const InspectorRow* b)
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
        float y = title_row->boundingRect().height();
        for (auto row : rows)
        {
            row->setPos(0, y);
            y += row->boundingRect().height() + PADDING_ROWS;
        }
    }

    prepareGeometryChange();
}
