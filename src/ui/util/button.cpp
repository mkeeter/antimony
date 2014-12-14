#include <QGraphicsSceneMouseEvent>

#include "ui/util/button.h"

GraphicsButton::GraphicsButton(QGraphicsItem *parent)
    : QGraphicsObject(parent), hover(false)
{
    setAcceptHoverEvents(true);
}

void GraphicsButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    hover = true;
    update();
}

void GraphicsButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    hover = false;
    update();
}

void GraphicsButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    if (event->button() == Qt::LeftButton)
        emit(pressed());
}

