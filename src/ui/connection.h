#ifndef CONNECTION_H
#define CONNECTION_H

#include <QGraphicsObject>
#include <QPointer>

#include "datum/link.h"

class Link;
class Canvas;
class Datum;
class Control;
class Node;

class Connection : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Connection(Link* link, Canvas* canvas);
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void setDragPos(QPointF p) { drag_pos = p; }

    Link* getLink() const { return link; }

public slots:
    void onPortPositionChanged() { prepareGeometryChange(); }

protected:

    /** Checks that start and end (if not dragging) datums are valid
     */
    bool areDatumsValid() const;

    Datum* startDatum() const;
    Datum* endDatum() const;

    Node* startNode() const;
    Node* endNode() const;

    Control* startControl() const;
    Control* endControl() const;

    /** Returns starting position in scene coordinates.
     */
    QPointF startPos() const;

    /** Returns ending position in scene coordinates.
     */
    QPointF endPos() const;

    /** Returns a path for the connection.
     */
    QPainterPath path() const;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    QPointer<Link> link;
    Canvas* canvas;
    QPointF drag_pos;

    enum { NONE, VALID, INVALID, CONNECTED } drag_state;
};

#endif // CONNECTION_H
