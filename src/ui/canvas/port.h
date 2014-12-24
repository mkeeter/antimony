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
    explicit Port(Datum* d, QGraphicsItem* parent);

    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    Datum* getDatum() const;

protected:
    QPointer<Datum> datum;
    bool hover;
};

class InputPort : public Port
{
public:
    explicit InputPort(Datum* d, QGraphicsItem* parent=NULL);
};

class OutputPort : public Port
{
public:
    explicit OutputPort(Datum* d, QGraphicsItem* parent=NULL);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
};

#endif // PORT_H
