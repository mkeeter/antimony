#include <Python.h>

#include <QPainter>

#include "canvas/inspector/row.h"
#include "canvas/inspector/frame.h"
#include "canvas/datum_editor.h"
#include "canvas/datum_port.h"

#include "app/colors.h"

#include "graph/datum.h"

const float InspectorRow::PORT_SIZE = 10;
const float InspectorRow::GAP_PADDING = 15;
const float InspectorRow::TEXT_WIDTH = 150;

InspectorRow::InspectorRow(Datum* d, InspectorFrame* parent)
    : QGraphicsObject(parent),
      input(new InputPort(d, this)), output(new OutputPort(d, this)),
      label(new QGraphicsTextItem(QString::fromStdString(d->getName()), this)),
      editor(new DatumEditor(d, this))
{
    label->setDefaultTextColor(Colors::base04);

    {   // Set port heights centered vertically
        const float port_height = (label->boundingRect().height() -
                                   input->boundingRect().height()) / 2;
        input->setPos(0, port_height);
        output->setPos(0, port_height);
    }

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

    output->setPos(boundingRect().right() - output->boundingRect().width(),
                   output->pos().y());
    prepareGeometryChange();
}

void InspectorRow::update(const DatumState& state)
{
    editor->update(state);
    static_cast<InspectorFrame*>(parentItem())->redoLayout();
}

bool InspectorRow::shouldBeHidden() const
{
    QString name = label->toPlainText();
    return name.startsWith("_") &&
          !name.startsWith("__");
}
