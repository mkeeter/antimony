#include <Python.h>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "app/colors.h"

#include "canvas/inspector/buttons.h"
#include "canvas/inspector/title.h"
#include "canvas/inspector/frame.h"

#include "graph/proxy/script.h"

////////////////////////////////////////////////////////////////////////////////

InspectorButton::InspectorButton(InspectorTitle* parent)
    : QGraphicsObject(parent), hover(false)
{
    setAcceptHoverEvents(true);
}

void InspectorButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    hover = true;
    update();
}

void InspectorButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    hover = false;
    update();
}

void InspectorButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    if (event->button() == Qt::LeftButton)
        emit(pressed());
}

////////////////////////////////////////////////////////////////////////////////

InspectorScriptButton::InspectorScriptButton(ScriptProxy* s,
                                             InspectorTitle* parent)
    : InspectorButton(parent)
{
    setToolTip("Edit script");
    connect(this, &InspectorButton::pressed,
            s, &ScriptProxy::newScriptWindow);

    parent->addButton(this);
}

QRectF InspectorScriptButton::boundingRect() const
{
    return QRectF(0, 0, 16, 15);
}

void InspectorScriptButton::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    const QColor base = script_valid ? Colors::base04 : Colors::red;
    painter->setBrush(hover ? Colors::highlight(base) : base);

    painter->drawRect(0, 0, 16, 3);
    painter->drawRect(0, 6, 16, 3);
    painter->drawRect(0, 12, 16, 3);
}

void InspectorScriptButton::setScriptValid(bool valid)
{
    if (valid != script_valid)
    {
        script_valid = valid;
        update();
    }
}

////////////////////////////////////////////////////////////////////////////////

InspectorShowHiddenButton::InspectorShowHiddenButton(InspectorFrame* frame)
    : InspectorButton(frame->getTitleRow()), toggled(false), inspector(frame)
{
    setToolTip("Show hidden datums");
    connect(this, &InspectorButton::pressed,
            [=](){ this->toggled = !this->toggled;
                   this->inspector->setShowHidden(this->toggled); });

    frame->getTitleRow()->addButton(this);
}

QRectF InspectorShowHiddenButton::boundingRect() const
{
    return QRectF(0, 0, 10, 15);
}

void InspectorShowHiddenButton::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    painter->setBrush(hover ? Colors::base06 :
                      toggled ? Colors::base04 : Colors::base02);
    painter->drawRect(0, 12, 10, 3);
    painter->drawEllipse(3, 4, 4, 4);
}
