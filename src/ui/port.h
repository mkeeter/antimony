#ifndef PORT_H
#define PORT_H

#include <QGraphicsObject>
#include <QPointer>

class NodeInspector;
class Datum;
class Canvas;

class Port : public QGraphicsObject
{
public:
    explicit Port(Datum* d, Canvas* canvas, QGraphicsItem* parent);

    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    Datum* getDatum() const;

    void fadeOut();
    void fadeIn();
protected:
    void setOpacity(float o);
    float getOpacity() const { return _opacity; }
    Q_PROPERTY(float opacity READ getOpacity write setOpacity);

    QPointer<Datum> datum;
    Canvas* canvas;
    float _opacity;
    bool hover;
};

class InputPort : public Port
{
public:
    explicit InputPort(Datum* d, Canvas* canvas, QGraphicsItem* parent=NULL);
};

class OutputPort : public Port
{
public:
    explicit OutputPort(Datum* d, Canvas* canvas, QGraphicsItem* parent=NULL);
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

#endif // PORT_H
