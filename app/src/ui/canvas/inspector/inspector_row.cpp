#include <Python.h>

#include "ui/canvas/inspector/inspector_row.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/inspector/inspector_text.h"
#include "ui/canvas/port.h"

#include "ui/util/colors.h"

#include "graph/datum.h"

const qreal InspectorRow::LeftPadding = 1;
const qreal InspectorRow::LabelPadding = 10; // padding between label & datum-text
const qreal InspectorRow::TextPadding = 5; // padding between datum-text & output-port

InspectorRow::InspectorRow(Datum* d, NodeInspector* parent)
    : QGraphicsObject(static_cast<QGraphicsItem*>(parent)),
      label(new QGraphicsTextItem(
                  QString::fromStdString(d->getName()), this)),
      editor(new DatumTextItem(d, this)),
      input(new InputPort(d, static_cast<QGraphicsItem*>(this))),
      output(new OutputPort(d, static_cast<QGraphicsItem*>(this)))
{
    label->setDefaultTextColor(Colors::base04);

    connect(editor,
            &DatumTextItem::boundsChanged,
            [=](){
            if(this->updateLayout())
                emit(layoutChanged()); });

    connect(static_cast<DatumTextItem*>(editor),
            &DatumTextItem::tabPressed,
            parent, &NodeInspector::focusNext);
    connect(static_cast<DatumTextItem*>(editor),
            &DatumTextItem::shiftTabPressed,
            parent, &NodeInspector::focusPrev);

    d->installWatcher(this);
    trigger(d->getState());
}

void InspectorRow::trigger(const DatumState& state)
{
    if (state.sigil == Datum::SIGIL_OUTPUT)
        input->hide();
    else
        input->show();
}

QRectF InspectorRow::boundingRect() const
{
    const float height = editor->boundingRect().height();
    const float width = 
        LeftPadding
        + Port::Width
        + labelWidth()    // Datum name
        + LabelPadding        // Padding
        + editor->boundingRect().width()    // Text field
        + TextPadding         // Padding
        + Port::Width;       // Output port
    return QRectF(0, 0, width, height);
}

float InspectorRow::minWidth() const
{
    return 
           LeftPadding
           + Port::Width
           + labelWidth() +  // Datum name
           + LabelPadding     // Padding
           + 150    // Text field
           + TextPadding      // Padding
           + Port::Width;    // Output port
}

void InspectorRow::setWidth(float width)
{
    editor->setTextWidth(150 + width - minWidth());
}

float InspectorRow::labelWidth() const
{
    Q_ASSERT(dynamic_cast<NodeInspector*>(parentObject()));
    return static_cast<NodeInspector*>(parentObject())->maxLabelWidth();
}

bool InspectorRow::updateLayout()
{
    float label_width = labelWidth();
    QRectF bbox = boundingRect();
    bool changed = false;

    if (input)
    {
        QPointF ipos(1 + LeftPadding, (bbox.height() - input->boundingRect().height()) / 2);
        if (input->pos() != ipos)
        {
            changed = true;
            input->setPos(ipos);
        }
    }

    QPointF lpos(LeftPadding + Port::Width + label_width - label->boundingRect().width(), // right justify
                 (bbox.height() - label->boundingRect().height())/2);
    if (label->pos() != lpos)
    {
        changed = true;
        label->setPos(lpos);
    }

    QPointF epos(LeftPadding + Port::Width + label_width + LabelPadding, 0);
    if (editor->pos() != epos)
    {
        changed = true;
        editor->setPos(epos);
    }

    if (output)
    {
        QPointF opos(bbox.width() - Port::Width,
                    (bbox.height() - output->boundingRect().height()) / 2);
        if (output->pos() != opos)
        {
            changed = true;
            output->setPos(opos);
        }
    }

    if (changed)
        prepareGeometryChange();
    return changed;
}

void InspectorRow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                         QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
    // Nothing to do here
}
