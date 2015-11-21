#include <Python.h>

#include <QPainter>

#include "canvas/inspector/row.h"
#include "canvas/inspector/frame.h"
#include "canvas/inspector/datum_editor.h"

#include "app/colors.h"

#include "graph/datum.h"

const float InspectorRow::PORT_SIZE = 10;
const float InspectorRow::GAP_PADDING = 15;
const float InspectorRow::TEXT_WIDTH = 150;

InspectorRow::InspectorRow(Datum* d, InspectorFrame* parent)
    : QGraphicsObject(parent), input(NULL), output(NULL),
      label(new QGraphicsTextItem(QString::fromStdString(d->getName()), this)),
      editor(new InspectorDatumEditor(d, this))
{
    label->setDefaultTextColor(Colors::base04);
    editor->setDefaultTextColor(Colors::base04);
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
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
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

void InspectorRow::update(const DatumState& state)
{
    editor->update(state);
}
