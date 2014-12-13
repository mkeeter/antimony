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
      label(new QGraphicsTextItem(d->objectName(), this))
{
    label->setDefaultTextColor(Colors::base04);

    editor = new DatumTextItem(d, this);
    connect(static_cast<DatumTextItem*>(editor),
            &DatumTextItem::boundsChanged,
            this, &InspectorRow::updateLayout);
    connect(static_cast<DatumTextItem*>(editor),
            &DatumTextItem::tabPressed,
            parent, &NodeInspector::focusNext);
    connect(static_cast<DatumTextItem*>(editor),
            &DatumTextItem::shiftTabPressed,
            parent, &NodeInspector::focusPrev);
}

QRectF InspectorRow::boundingRect() const
{
    float height = editor->boundingRect().height();

    float width = 15 + globalLabelWidth() + 10 + 150 + 5 + 15;
    return QRectF(0, 0, width, height);
}

float InspectorRow::globalLabelWidth() const
{
   return dynamic_cast<NodeInspector*>(parentObject())->labelWidth();
}

void InspectorRow::updateLayout()
{
    float label_width = globalLabelWidth();
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

    QPointF epos(label_width + 25, 0);
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

    if (changed)
    {
        emit(layoutChanged());
    }
}

void InspectorRow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                         QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
    // Nothing to do here
}
