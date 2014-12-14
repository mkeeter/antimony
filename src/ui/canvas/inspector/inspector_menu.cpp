#include <Python.h>

#include <QPainter>

#include "ui/canvas/inspector/inspector_menu.h"
#include "ui/canvas/inspector/inspector.h"

#include "ui/util/colors.h"

InspectorMenuButton::InspectorMenuButton(NodeInspector* parent)
    : GraphicsButton(parent)
{
    // Nothing to do here
}

QRectF InspectorMenuButton::boundingRect() const
{
    return QRectF(0, 0, 20, 20);
}

void InspectorMenuButton::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    painter->setBrush(hover ? Colors::base05 : Colors::base04);
    painter->drawRect(2, 1, 16, 3);
    painter->drawRect(2, 7, 16, 3);
    painter->drawRect(2, 13, 16, 3);
}
