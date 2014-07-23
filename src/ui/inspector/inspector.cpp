#include <Python.h>

#include <QDebug>

#include <QPropertyAnimation>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QTimer>

#include "app.h"
#include "ui/main_window.h"

#include "ui/canvas.h"
#include "ui/inspector/inspector.h"
#include "ui/inspector/inspector_button.h"
#include "ui/inspector/inspector_text.h"
#include "ui/port.h"

#include "datum/datum.h"
#include "datum/script_datum.h"
#include "control/control.h"
#include "node/node.h"

//////////////////////////////////////////r/////////////////////////////////////

NodeInspector::NodeInspector(Control* control)
    : control(control)
{
    connect(control, SIGNAL(inspectorPositionChanged()),
            this, SLOT(onPositionChange()));
    connect(control->getCanvas(), SIGNAL(viewChanged()),
            this, SLOT(onPositionChange()));

    connect(control, SIGNAL(destroyed()),
            this, SLOT(animateClose()));

    populateLists(control->getNode());
    setZValue(-2);
    setFlag(ItemClipsChildrenToShape);

    onPositionChange();
    control->scene()->addItem(this);
    animateOpen();
}

Canvas* NodeInspector::getCanvas() const
{
    return control->getCanvas();
}

float NodeInspector::labelWidth() const
{
    float label_width = 0;
    for (auto label : labels)
    {
        label_width = fmax(label_width, label->boundingRect().width());
    }
    return label_width;
}

QRectF NodeInspector::boundingRect() const
{
    float height = 0;
    float width = 15 + labelWidth() + 10 + 150 + 5 + 15;

    for (auto e : editors)
    {
        height += e->boundingRect().height() + 6;
    }
    return QRectF(0, 0, width*mask_size, height*mask_size);
}

void NodeInspector::onLayoutChanged()
{
    float label_width = labelWidth();
    float x = 15;
    float y = 0;
    float xo = boundingRect().width() - 10;
    for (int i=0; i < labels.length(); ++i)
    {
        float ey = editors[i]->boundingRect().height();
        float ly = labels[i] ->boundingRect().height();
        if (inputs[i])
        {
            float iy = inputs[i]->boundingRect().height();
            inputs[i]->setPos(0, (y + (ey - iy) / 2 + 3) * mask_size);
        }
        labels[i]->setPos(x + label_width - labels[i]->boundingRect().width(),
                          y + (ey - ly) / 2 + 3);
        editors[i]->setPos(x + label_width + 10, y + 3);
        if (outputs[i])
        {
            float oy = outputs[i]->boundingRect().height();
            outputs[i]->setPos(xo * mask_size,
                               (y + (ey - oy) / 2 + 3) * mask_size);
        }
        y += fmax(ey, ly) + 6;
    }
    prepareGeometryChange();
}

void NodeInspector::populateLists(Node *node)
{
    for (Datum* d : node->findChildren<Datum*>())
    {
        if (d->parent() != node || d->objectName().startsWith("_"))
        {
            continue;
        }
        inputs << (d->hasInput() ? new InputPort(d, this) : NULL);
        labels << new QGraphicsTextItem(d->objectName(), this);

        if (dynamic_cast<ScriptDatum*>(d))
        {
            DatumTextButton* b = new DatumTextButton(d, "Open script", this);
            editors << b;
            connect(b, SIGNAL(pressed(Datum*)), this, SLOT(openScript(Datum*)));
        }
        else
        {
            editors << new DatumTextItem(d, this);
            connect(editors.back(), SIGNAL(boundsChanged()),
                    this, SLOT(onLayoutChanged()));
        }
        outputs << (d->hasOutput() ? new OutputPort(d, this) : NULL);

    }
    onLayoutChanged();
}

void NodeInspector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(QColor("#ddd"));
    painter->setPen(Qt::NoPen);
    painter->drawRect(boundingRect());
}

void NodeInspector::animateClose()
{
    // Take focus away from text entry box to prevent graphical artifacts
    QPropertyAnimation* a = new QPropertyAnimation(this, "mask_size", this);
    a->setDuration(100);
    a->setStartValue(1);
    a->setEndValue(0);
    connect(a, SIGNAL(finished()), this, SLOT(deleteLater()));
    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void NodeInspector::animateOpen()
{
    QPropertyAnimation* a = new QPropertyAnimation(this, "mask_size", this);
    a->setDuration(100);
    a->setStartValue(0);
    a->setEndValue(1);
    a->start(QPropertyAnimation::DeleteWhenStopped);
}

float NodeInspector::getMaskSize() const
{
    return mask_size;
}

void NodeInspector::setMaskSize(float m)
{
    mask_size = m;
    for (auto p : inputs)
    {
        if (p)
            p->setOpacity(mask_size);
    }
    for (auto p : outputs)
    {
        if (p)
            p->setOpacity(mask_size);
    }
    onLayoutChanged();
    prepareGeometryChange();
    emit portPositionChanged();
}

void NodeInspector::onPositionChange()
{
    if (control)
    {
        setPos(control->inspectorPosition());
    }
}


void NodeInspector::openScript(Datum *d) const
{
    ScriptDatum* s = dynamic_cast<ScriptDatum*>(d);
    Q_ASSERT(s);
    App::instance()->getWindow()->openScript(s);
}
