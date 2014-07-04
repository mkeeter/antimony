#ifndef PORT_H
#define PORT_H

#include <QGraphicsObject>

class NodeInspector;
class Datum;

class Port : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Port(Datum* d, NodeInspector* inspector);

    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
    void setOpacity(float o) { opacity = o; update(); }
    Datum* getDatum() { return datum; }
protected:
    Datum* datum;
    float opacity;
    bool hover;
};

class InputPort : public Port
{
    Q_OBJECT
public:
    explicit InputPort(Datum* d, NodeInspector* inspector);
};

class OutputPort : public Port
{
    Q_OBJECT
public:
    explicit OutputPort(Datum* d, NodeInspector* inspector);
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

#endif // PORT_H
