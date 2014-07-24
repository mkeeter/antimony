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
#include "ui/inspector/inspector_row.h"
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
    for (auto row : rows)
    {
        label_width = fmax(label_width, row->label->boundingRect().width());
    }
    return label_width;
}

QRectF NodeInspector::boundingRect() const
{
    float height = 0;
    float width = 0;

    for (auto row : rows)
    {
        height += row->boundingRect().height() + 6;
        width = fmax(width, row->boundingRect().width());
    }
    return QRectF(0, 0, width*mask_size, height*mask_size);
}

void NodeInspector::onLayoutChanged()
{
    float y = 3;
    for (auto row : rows)
    {
        row->setPos(0, y);
        y += 6 + row->boundingRect().height();
    }
    prepareGeometryChange();
}

void NodeInspector::populateLists(Node *node)
{
    for (Datum* d : node->findChildren<Datum*>())
    {
        rows[d] = new InspectorRow(d, this);
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

InputPort* NodeInspector::datumInputPort(Datum *d) const
{
    for (auto row : rows)
    {
        for (auto a : row->childItems())
        {
            InputPort* p = dynamic_cast<InputPort*>(a);
            if (p && p->getDatum() == d)
            {
                return p;
            }
        }
    }
    return NULL;
}

OutputPort* NodeInspector::datumOutputPort(Datum *d) const
{
    for (auto row : rows)
    {
        for (auto a : row->childItems())
        {
            OutputPort* p = dynamic_cast<OutputPort*>(a);
            if (p && p->getDatum() == d)
            {
                return p;
            }
        }
    }
    return NULL;
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

    for (auto row : rows)
    {
        row->setPortOpacity(mask_size);
        row->updateLayout();
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
