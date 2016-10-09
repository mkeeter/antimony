#include <Python.h>

#include <QPainter>

#include "canvas/inspector/frame.h"

#include "canvas/datum_row.h"
#include "canvas/datum_editor.h"
#include "canvas/datum_port.h"

#include "app/colors.h"

#include "graph/datum.h"

const float DatumRow::PORT_SIZE = 10;
const float DatumRow::GAP_PADDING = 15;
const float DatumRow::TEXT_WIDTH = 150;

////////////////////////////////////////////////////////////////////////////////

DatumRow::DatumRow(Datum* d, QGraphicsItem* parent)
    : QGraphicsObject(parent),
      input(new InputPort(d, this)), output(new OutputPort(d, this)),
      label(new QGraphicsTextItem(QString::fromStdString(d->getName()), this)),
      editor(NULL)
{
    label->setDefaultTextColor(Colors::base04);

    {   // Set port heights centered vertically
        const float port_height = (label->boundingRect().height() -
                                   input->boundingRect().height()) / 2;
        input->setPos(0, port_height);
        output->setPos(0, port_height);
    }
}

DatumRow::DatumRow(Datum* d, InspectorFrame* parent)
    : DatumRow(d, static_cast<QGraphicsItem*>(parent))
{
    connect(this, &DatumRow::layoutChanged,
            parent, &InspectorFrame::redoLayout);
    editor = new DatumEditor(d, this);

    connect(editor, &DatumEditor::tabPressed,
            parent, &InspectorFrame::focusNext);
    connect(editor, &DatumEditor::shiftTabPressed,
            parent, &InspectorFrame::focusPrev);
}

////////////////////////////////////////////////////////////////////////////////

QRectF DatumRow::boundingRect() const
{
    const float height = editor->boundingRect().height();
    const float width = editor->pos().x() + editor->boundingRect().width() +
                        GAP_PADDING + PORT_SIZE;

    return QRectF(0, 0, width, height);
}

void DatumRow::paint(QPainter* painter,
                     const QStyleOptionGraphicsItem* option,
                     QWidget* widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

float DatumRow::labelWidth() const
{
    return label->boundingRect().width();
}

float DatumRow::minWidth() const
{
    return label->pos().x() + labelWidth() +
           GAP_PADDING +
           TEXT_WIDTH +
           GAP_PADDING + PORT_SIZE;
}

void DatumRow::padLabel(float width)
{
    label->setPos(PORT_SIZE + GAP_PADDING + width - labelWidth(), 0);
    prepareGeometryChange();
}

void DatumRow::setWidth(float width)
{
    editor->setTextWidth(TEXT_WIDTH + (width - minWidth()));
    editor->setPos(label->pos().x() + labelWidth() + GAP_PADDING, 0);

    output->setPos(boundingRect().right() - output->boundingRect().width(),
                   output->pos().y());
    prepareGeometryChange();
}

////////////////////////////////////////////////////////////////////////////////

void DatumRow::update(const DatumState& state)
{
    auto bbox = boundingRect();
    editor->update(state);
    bool changed = bbox != boundingRect();

    if (state.sigil == Datum::SIGIL_OUTPUT ||
        state.sigil == Datum::SIGIL_SUBGRAPH_OUTPUT ||
        state.sigil == Datum::SIGIL_SUBGRAPH_CONNECTION)
    {
        input->hide();
        changed = true;
    }
    else
    {
        input->show();
        changed = true;
    }

    if (changed)
    {
        emit(layoutChanged());
    }
}

////////////////////////////////////////////////////////////////////////////////

bool DatumRow::shouldBeHidden() const
{
    QString name = label->toPlainText();
    return name.startsWith("_") &&
          !name.startsWith("__");
}
