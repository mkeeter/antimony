#ifndef CONNECTION_H
#define CONNECTION_H

#include <QGraphicsObject>

class Link;
class Canvas;
class Datum;
class Control;

class Connection : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Connection(Link* link, Canvas* canvas);
    ~Connection();
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void setDragPos(QPointF p) { drag_pos = p; }
public slots:
    void onPortPositionChanged() { prepareGeometryChange(); }
protected:

    Datum* startDatum() const;
    Datum* endDatum() const;

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

    Link* link;
    Canvas* canvas;
    QPointF drag_pos;

    enum { NONE, VALID, INVALID } drag_state;
};

#endif // CONNECTION_H
