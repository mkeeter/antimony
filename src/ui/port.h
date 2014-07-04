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

protected:
    Datum* datum;
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
};

#endif // PORT_H
