#ifndef GRAPHICS_BUTTON_H
#define GRAPHICS_BUTTON_H

#include <QGraphicsObject>

class GraphicsButton : public QGraphicsObject
{
    Q_OBJECT
public:
    GraphicsButton(QGraphicsItem* parent);
signals:
    void pressed();
public slots:
    void redraw() { update(); }
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    bool hover;
};

#endif // GRAPHICS_BUTTON_H
