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
    explicit Port(Datum* d, QGraphicsItem* parent);
    virtual ~Port() {}

    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    Datum* getDatum() const;

    QVariant itemChange(GraphicsItemChange change,
                        const QVariant& value) override;
signals:
    void moved();
    void hiddenChanged();

protected:
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
protected:
    QMap<const Datum*, Connection*> connections;
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
