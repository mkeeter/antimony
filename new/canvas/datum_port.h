#pragma once

#include <QGraphicsObject>
#include <QPointer>
#include <QSet>

#include "graph/watchers.h"

class Datum;
class Canvas;
class Connection;
class DatumRow;

////////////////////////////////////////////////////////////////////////////////

class DatumPort : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit DatumPort(Datum* d, DatumRow* parent);

    /*
     *  QGraphicsItem functions
     */
    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    /*
     *  Look up the datum associated with this port
     */
    Datum* getDatum() const { return datum; }

    /*
     *  Overload itemChange to emit moved() and hiddenChanged() signals
     */
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant& value) override;

    /*
     *  Returns the target rectangle for the port
     *
     *  (overloaded by child classes to snap to left or right side
     *   of the bounding rectangle)
     */
    virtual QRectF portRect() const=0;

signals:
    /*
     *  Indicate that the port has moved (to redraw connections)
     */
    void moved();

    /*
     *  Indicate that the port has become hidden (to hide connections)
     */
    void hiddenChanged();

protected:
    Datum* const datum;

    //  Flag to indicate whether the port should be highlighted
    bool hover;

    // Background color (specific to the datum type)
    const QColor color;
};

////////////////////////////////////////////////////////////////////////////////

class InputPort : public DatumPort
{
public:
    explicit InputPort(Datum* d, DatumRow* parent);

    /*
     *  Handle dropping a connection on the given port
     */
    void install(Connection* c);

    /*
     *  Input port rectangles are on the left
     */
    QRectF portRect() const override;
};

////////////////////////////////////////////////////////////////////////////////

class OutputPort : public DatumPort
{
public:
    explicit OutputPort(Datum* d, DatumRow* parent);

    /*
     *  Output port rectangles are on the right
     */
    QRectF portRect() const override;

protected:
    /*
     *  Handle mouse events to create outgoing ports and hover highlighting
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
};
