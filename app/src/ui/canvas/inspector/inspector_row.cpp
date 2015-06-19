#include <Python.h>

#include "ui/canvas/inspector/inspector_row.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/inspector/inspector_text.h"
#include "ui/canvas/port.h"

#include "ui/util/colors.h"

#include "graph/datum/datum.h"
#include "graph/datum/datums/script_datum.h"

InspectorRow::InspectorRow(Datum* d, NodeInspector* parent)
    : QGraphicsObject(static_cast<QGraphicsItem*>(parent)),
      input(d->hasInput()
                ? new InputPort(d, static_cast<QGraphicsItem*>(this))
                : NULL),
      output(d->hasOutput()
                ? new OutputPort(d, static_cast<QGraphicsItem*>(this))
                : NULL),
      label(new QGraphicsTextItem(d->objectName(), this)),
      editor(new DatumTextItem(d, this))
{
    label->setDefaultTextColor(Colors::base04);

    connect(static_cast<DatumTextItem*>(editor),
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
}

QRectF InspectorRow::boundingRect() const
{
    const float height = editor->boundingRect().height();
    const float width = 15      // Input port
        + labelWidth()    // Datum name
        + 10        // Padding
        + editor->boundingRect().width()    // Text field
        + 5         // Padding
        + 15;       // Output port
    return QRectF(0, 0, width, height);
}

float InspectorRow::minWidth() const
{
    return 15       // Input port
           + labelWidth() +  // Datum name
           + 10     // Padding
           + 150    // Text field
           + 5      // Padding
           + 15;    // Output port
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
        QPointF ipos(1, (bbox.height() - input->boundingRect().height()) / 2);
        if (input->pos() != ipos)
        {
            changed = true;
            input->setPos(ipos);
        }
    }

    QPointF lpos(15 + label_width - label->boundingRect().width(),
                 (bbox.height() - label->boundingRect().height())/2);
    if (label->pos() != lpos)
    {
        changed = true;
        label->setPos(lpos);
    }

    QPointF epos(15 + label_width + 10, 0);
    if (editor->pos() != epos)
    {
        changed = true;
        editor->setPos(epos);
    }

    if (output)
    {
        QPointF opos(bbox.width() - output->boundingRect().width() - 1,
                    (bbox.height() - output->boundingRect().height()) / 2);
        if (output->pos() != opos)
        {
            changed = true;
            output->setPos(opos);
        }
    }

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
