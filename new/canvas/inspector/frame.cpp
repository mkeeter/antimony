#include <Python.h>

#include <QGraphicsScene>
#include <QPainter>

#include "canvas/inspector/frame.h"
#include "app/colors.h"

InspectorFrame::InspectorFrame(Node* node, QGraphicsScene* scene)
    : QGraphicsObject()
{
    setFlags(QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    Q_UNUSED(node);

    scene->addItem(this);
}

QRectF InspectorFrame::boundingRect() const
{
    return QRectF(0, 0, 100, 100);
}

void InspectorFrame::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
    const auto r = boundingRect();

    // Draw interior
    painter->setBrush(Colors::base01);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(r, 8, 8);

    // Draw outer edge
    painter->setBrush(Qt::NoBrush);
    if (isSelected())
        painter->setPen(QPen(Colors::base05, 2));
    else
        painter->setPen(QPen(Colors::base03, 2));
    painter->drawRoundedRect(r, 8, 8);
}

////////////////////////////////////////////////////////////////////////////////

void InspectorFrame::setNameValid(bool valid)
{
}

void InspectorFrame::setScriptValid(bool valid)
{
}

////////////////////////////////////////////////////////////////////////////////

void InspectorFrame::setTitle(QString title)
{
}

////////////////////////////////////////////////////////////////////////////////

void InspectorFrame::redoLayout()
{
}
