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
    virtual ~Port();

    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    Datum* getDatum() const;

    void hideToolTip();
    void showToolTip();
    void setPos(QPointF pos);

public slots:
    void setVisible(bool v);

protected:
    void fadeOut();
    void fadeIn();

    void setOpacity(float o);
    float getOpacity() const { return _opacity; }
    Q_PROPERTY(float opacity READ getOpacity WRITE setOpacity);

    QPointer<Datum> datum;
    Canvas* canvas;
    QPointer<QGraphicsTextItem> label;

    QPointF label_offset;
    float _opacity;
    bool hover;

    bool visible;
    bool visible_g;
};

class InputPort : public Port
{
public:
    explicit InputPort(Datum* d, Canvas* canvas, QGraphicsItem* parent=NULL);
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
};

class OutputPort : public Port
{
public:
    explicit OutputPort(Datum* d, Canvas* canvas, QGraphicsItem* parent=NULL);
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // PORT_H
