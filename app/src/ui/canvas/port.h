#ifndef PORT_H
#define PORT_H

#include <QGraphicsObject>
#include <QPointer>
#include <QSet>

#include "graph/watchers.h"

class NodeInspector;
class Datum;
class Canvas;
class Connection;

class Port : public QGraphicsObject
{
    Q_OBJECT
public:
    static const qreal Width; // The width of the actual "drag/drop" graphic-area
    static const qreal PaintSize; // The x,y size of the painted spot

    explicit Port(Datum* d, QGraphicsItem* parent);
    virtual ~Port() {}

    virtual QRectF boundingRect() const override =0;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    QRectF dropRect() const {return paintRect(); }

    Datum* getDatum() const;

    QVariant itemChange(GraphicsItemChange change,
                        const QVariant& value) override;
signals:
    void moved();
    void hiddenChanged();

protected:
    virtual qreal adjust_left_right() const;
    QRectF paintRect() const;

    Datum* datum;
    bool hover;
    const QColor color;
};

class InputPort : public Port, DatumWatcher
{
public:
    explicit InputPort(Datum* d, QGraphicsItem* parent=NULL);
    virtual ~InputPort();
    void trigger(const DatumState& state) override;
    void install(Connection* c);
    QRectF boundingRect() const;


protected:
    QMap<const Datum*, Connection*> connections;
};

class OutputPort : public Port
{
public:
    explicit OutputPort(Datum* d, QGraphicsItem* parent=NULL);
    QRectF boundingRect() const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    qreal adjust_left_right() const;
};

#endif // PORT_H
