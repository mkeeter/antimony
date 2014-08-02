#ifndef PORT_H
#define PORT_H

#include <QGraphicsObject>
#include <QPointer>

class NodeInspector;
class Datum;
class Canvas;

class Port : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Port(Datum* d, Canvas* canvas, QGraphicsItem* parent);

    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    void setOpacity(float o) { opacity = o; update(); }
    Datum* getDatum() const;
protected:
    QPointer<Datum> datum;
    Canvas* canvas;
    float opacity;
    bool hover;
};

class InputPort : public Port
{
    Q_OBJECT
public:
    explicit InputPort(Datum* d, Canvas* canvas, QGraphicsItem* parent);
};

class OutputPort : public Port
{
    Q_OBJECT
public:
    explicit OutputPort(Datum* d, Canvas* canvas, QGraphicsItem* parent);
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

#endif // PORT_H
