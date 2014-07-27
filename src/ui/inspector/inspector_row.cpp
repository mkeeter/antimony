#include <Python.h>

#include "ui/inspector/inspector_row.h"
#include "ui/inspector/inspector.h"
#include "ui/inspector/inspector_button.h"
#include "ui/inspector/inspector_text.h"
#include "ui/port.h"
#include "ui/colors.h"

#include "datum/datum.h"
#include "datum/script_datum.h"

InspectorRow::InspectorRow(Datum* d, NodeInspector* parent)
    : QGraphicsObject(static_cast<QGraphicsItem*>(parent)),
      input(d->hasInput() ? new InputPort(d, parent->getCanvas(),
                                          static_cast<QGraphicsItem*>(this))
                          : NULL),
      output(d->hasOutput() ? new OutputPort(d, parent->getCanvas(),
                                             static_cast<QGraphicsItem*>(this))
                            : NULL),
      label(new QGraphicsTextItem(d->objectName(), this))
{
    label->setDefaultTextColor(Colors::base04);

    if (dynamic_cast<ScriptDatum*>(d))
    {
        editor = new DatumTextButton(d, "Open script", this);
        connect(editor, SIGNAL(pressed(Datum*)),
                parent, SLOT(openScript(Datum*)));
    }
    else
    {
        editor = new DatumTextItem(d, this);
        connect(editor, SIGNAL(boundsChanged()),
                this, SLOT(updateLayout()));
    }
}

QRectF InspectorRow::boundingRect() const
{
    float height = editor->boundingRect().height();

    float width = 15 + globalLabelWidth() + 10 + 150 + 5 + 15;
    return QRectF(0, 0, width, height);
}

void InspectorRow::setPortOpacity(float o)
{
    if (input)
    {
        input->setOpacity(o);
    }
    if (output)
    {
        output->setOpacity(o);
    }
}

float InspectorRow::globalLabelWidth() const
{
   return dynamic_cast<NodeInspector*>(parentObject())->labelWidth();
}

void InspectorRow::updateLayout()
{
    float label_width = globalLabelWidth();
    QRectF bbox = boundingRect();

    if (input)
    {
        input->setPos(0, (bbox.height() - input->boundingRect().height()) / 2);
    }

    label->setPos(15 + label_width - label->boundingRect().width(),
                  (bbox.height() - label->boundingRect().height())/2);

    editor->setPos(label_width + 25, 0);
    if (output)
    {
        output->setPos(bbox.width() - output->boundingRect().width(),
                       (bbox.height() - output->boundingRect().height()) / 2);
    }
    emit(layoutChanged());
}

void InspectorRow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                         QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
    // Nothing to do here
}
