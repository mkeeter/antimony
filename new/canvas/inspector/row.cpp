#include <Python.h>

#include <QPainter>

#include "canvas/inspector/row.h"
#include "canvas/inspector/frame.h"

#include "app/colors.h"

const float InspectorRow::PORT_SIZE = 10;
const float InspectorRow::GAP_PADDING = 15;
const float InspectorRow::TEXT_WIDTH = 150;

InspectorRow::InspectorRow(QString name, InspectorFrame* parent)
    : QGraphicsObject(parent), input(NULL), output(NULL),
      label(new QGraphicsTextItem(name, this)),
      editor(new QGraphicsTextItem("", this))
{

    label->setDefaultTextColor(Colors::base04);
    editor->setDefaultTextColor(Colors::base04);
    // Nothing to do here
}

QRectF InspectorRow::boundingRect() const
{
    const float height = editor->boundingRect().height();
    const float width = editor->pos().x() + editor->boundingRect().width() +
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
    painter->drawRect(label->boundingRect().translated(label->pos()));
    painter->setBrush(Qt::blue);
    painter->drawRect(editor->boundingRect().translated(editor->pos()));
}

float InspectorRow::labelWidth() const
{
    return label->boundingRect().width();
}

float InspectorRow::minWidth() const
{
    return label->pos().x() + labelWidth() +
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
    editor->setPos(label->pos().x() + labelWidth() + GAP_PADDING, 0);
    prepareGeometryChange();
}

void InspectorRow::setText(QString t)
{
    editor->setPlainText(t);
}
