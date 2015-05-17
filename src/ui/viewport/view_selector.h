#ifndef VIEW_SELECTOR_H
#define VIEW_SELECTOR_H

#include <QGraphicsObject>

class Viewport;

class ViewSelectorButton : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit ViewSelectorButton(QString label, QPointF pos,
                                QGraphicsItem* parent);
signals:
    void mouseEnter(QString label);
    void mouseLeave();
    void pressed();
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    void mousePressEvent(QGraphicsSceneMouseEvent* event);

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    QString label;
};

class ViewSelector : public QGraphicsTextItem
{
    Q_OBJECT
public:
    explicit ViewSelector(Viewport* viewport);

public slots:
    void setLabel(QString label);
    void clearLabel();

    void onTopPressed();
    void onFrontPressed();
    void onLeftPressed();
    void onRightPressed();
    void onBackPressed();
    void onBottomPressed();
    void onPerspectivePressed();
signals:
    void spinTo(float pitch, float yaw);
};

#endif // VIEW_SELECTOR_H
