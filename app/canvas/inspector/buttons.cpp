#include <Python.h>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "app/colors.h"

#include "canvas/inspector/buttons.h"
#include "canvas/inspector/title.h"
#include "canvas/inspector/frame.h"

#include "graph/proxy/script.h"
#include "graph/proxy/graph.h"

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

////////////////////////////////////////////////////////////////////////////////

InspectorGraphButton::InspectorGraphButton(GraphProxy* n, InspectorTitle* parent)
    : InspectorButton(parent)
{
    setToolTip("Edit graph");
    connect(this, &InspectorButton::pressed,
            n, &GraphProxy::newCanvasWindow);

    parent->addButton(this);
}

QRectF InspectorGraphButton::boundingRect() const
{
    return QRectF(0, 0, 16, 15);
}

void InspectorGraphButton::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    const QColor base = Colors::base04;
    painter->setBrush(hover ? Colors::highlight(base) : base);

    painter->drawEllipse(0, 5, 5, 5);
    painter->drawEllipse(10, 11, 5, 5);
    painter->drawEllipse(10, 0, 5, 5);

    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(hover ? Colors::highlight(base) : base, 2));
    painter->drawLine(QLineF(2.5, 7.5, 12.5, 13.5));
    painter->drawLine(QLineF(2.5, 7.5, 12.5, 2.5));
}

////////////////////////////////////////////////////////////////////////////////

InspectorViewButton::InspectorViewButton(GraphProxy* n, InspectorTitle* parent)
    : InspectorButton(parent)
{
    setToolTip("Open viewport");
    connect(this, &InspectorButton::pressed,
            n, &GraphProxy::newViewportWindow);
    parent->addButton(this);
}

QRectF InspectorViewButton::boundingRect() const
{
    return QRectF(0, 0, 16, 15);
}

void InspectorViewButton::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(Qt::NoBrush);
    const QColor base = Colors::base04;
    painter->setPen(QPen(hover ? Colors::highlight(base) : base, 2));

    painter->drawRect(1, 15, 15, 1);
    painter->drawLine(1, 15, 1, 1);
    painter->drawLine(1, 15, 8, 8);
}

////////////////////////////////////////////////////////////////////////////////

InspectorQuadButton::InspectorQuadButton(GraphProxy* n, InspectorTitle* parent)
    : InspectorButton(parent)
{
    setToolTip("Open quad window");
    connect(this, &InspectorButton::pressed,
            n, &GraphProxy::newQuadWindow);
    parent->addButton(this);
}

QRectF InspectorQuadButton::boundingRect() const
{
    return QRectF(0, 0, 16, 16);
}

void InspectorQuadButton::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(Qt::NoBrush);
    const QColor base = Colors::base04;
    painter->setPen(QPen(hover ? Colors::highlight(base) : base, 2));

    painter->drawRect(1, 1, 14, 14);
    painter->drawLine(1, 8, 15, 8);
    painter->drawLine(8, 1, 8, 15);
}
