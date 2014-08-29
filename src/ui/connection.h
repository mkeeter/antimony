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
class NodeInspector;

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
    void onPortPositionChanged();

protected:

    /** Checks that start and end (if not dragging) datums are valid
     */
    bool areDatumsValid() const;
    bool areNodesValid() const;
    bool areControlsValid() const;

    /** Look up start and end datums.
     */
    Datum* startDatum() const;
    Datum* endDatum() const;

    /** Look up start and end nodes.
     */
    Node* startNode() const;
    Node* endNode() const;

    /** Look up start and end controls.
     */
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

    /** While the connection is open-ended, check for target ports.
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    /** On mouse release, connect to an available port if not already connected.
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    /** Check for mouse hover.
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    QPointer<Link> link;
    Canvas* canvas;
    QPointF drag_pos;

    enum { NONE, VALID, INVALID, CONNECTED } drag_state;

    NodeInspector* raised_inspector;
    bool hover;
};

#endif // CONNECTION_H
