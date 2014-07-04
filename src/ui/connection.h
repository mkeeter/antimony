#ifndef CONNECTION_H
#define CONNECTION_H

#include <QGraphicsObject>

class Link;
class Canvas;

class Connection : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Connection(Link* link, Canvas* canvas);
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void setDragPos(QPointF p) { drag_pos = p; }
protected:
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
};

#endif // CONNECTION_H
