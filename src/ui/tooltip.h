#ifndef TOOLTIP_H
#define TOOLTIP_H

#include <QGraphicsTextItem>

class ToolTipItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    ToolTipItem(QString txt, QGraphicsItem* parent=NULL);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem*,
               QWidget*) override;
};

#endif
