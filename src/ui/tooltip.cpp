#include <QPainter>

#include "ui/tooltip.h"
#include "ui/colors.h"

ToolTipItem::ToolTipItem(QString txt, QGraphicsItem* parent)
    : QGraphicsTextItem(txt, parent)
{
    setDefaultTextColor(Colors::base03);
    setZValue(10);
}

void ToolTipItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* o,
                        QWidget* w)
{
    painter->setBrush(Colors::base04);
    painter->setPen(QPen(Colors::base04, 1));
    painter->drawRect(boundingRect());
    QGraphicsTextItem::paint(painter, o, w);

}
