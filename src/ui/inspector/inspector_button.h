#ifndef INSPECTOR_BUTTON_H
#define INSPECTOR_BUTTON_H

#include <QGraphicsTextItem>

class Datum;

class DatumTextButton : public QGraphicsTextItem
{
    Q_OBJECT
public:
    DatumTextButton(Datum* datum, QString label, QGraphicsItem* parent);
signals:
    void pressed(Datum* d);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    Datum* d;
    bool hover;
    QColor background;
};

#endif // INSPECTOR_BUTTON_H
