#include <Python.h>

#include <QPainter>

#include "canvas/inspector/row.h"
#include "canvas/inspector/frame.h"

const float InspectorRow::PORT_SIZE = 10;
const float InspectorRow::GAP_PADDING = 15;
const float InspectorRow::TEXT_WIDTH = 150;

InspectorRow::InspectorRow(QString name, InspectorFrame* parent)
    : QGraphicsObject(parent), input(NULL), output(NULL),
      label(new QGraphicsTextItem(name, this)),
      editor(new QGraphicsTextItem(name, this))
{
    connect(this, &InspectorRow::layoutChanged,
            parent, &InspectorFrame::redoLayout);
    emit(layoutChanged());
}

QRectF InspectorRow::boundingRect() const
{
    const float height = editor->boundingRect().height();
    const float width = PORT_SIZE + GAP_PADDING +
                        labelWidth() + label_padding +
                        GAP_PADDING +
                        editor->boundingRect().width() +
                        GAP_PADDING + PORT_SIZE;

    return QRectF(0, 0, width, height);
}

void InspectorRow::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem* option,
                         QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(Qt::red);
    painter->drawRect(boundingRect());
}

float InspectorRow::labelWidth() const
{
    return label->boundingRect().width();
}

float InspectorRow::minWidth() const
{
    return label->pos().x() + labelWidth() +
           labelWidth() + label_padding +
           GAP_PADDING +
           TEXT_WIDTH +
           GAP_PADDING + PORT_SIZE;
}

void InspectorRow::padLabel(float width)
{
    label->setPos(PORT_SIZE + GAP_PADDING + width - labelWidth(), 0);
    prepareGeometryChange();
}

void InspectorRow::setWidth(float width)
{
    editor->setTextWidth(TEXT_WIDTH + (width - minWidth()));
    prepareGeometryChange();
}
