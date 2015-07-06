#ifndef CONNECTION_H
#define CONNECTION_H

#include <QGraphicsObject>
#include <QPointer>

#include "graph/watchers.h"

class Datum;
class Node;
class InputPort;
class GraphScene;
class NodeInspector;

class Connection : public QGraphicsObject, NodeWatcher, DatumWatcher
{
    Q_OBJECT
public:
    explicit Connection(Datum* datum);
    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void setDragPos(QPointF p) { drag_pos = p; }

    /* Makes connections between inspector port signals and redraw.
     */
    void makeSceneConnections();

public slots:
    void onInspectorMoved();
    void onHiddenChanged();

protected:
    GraphScene* gscene() const;

    /** On shift key press, snap to the nearest node.
     */
    void keyPressEvent(QKeyEvent *event) override;

    /** On shift key release, stop snapping
     */
    void keyReleaseEvent(QKeyEvent* event) override;

    /** Updates snap_pos.
     */
    void updateSnap();

    /** Checks that start and end (if not dragging) datums are valid
     */
    bool areDatumsValid() const;
    bool areNodesValid() const;
    bool areInspectorsValid() const;
    bool isHidden() const;

    /** Checks to see whether we're on a valid port
     *  (and adjust drag_state accordingly).
     */
    void checkDragTarget();

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
    NodeInspector* startInspector() const;
    NodeInspector* endInspector() const;

    /** Returns starting position in scene coordinates.
     */
    QPointF startPos() const;

    /** Returns ending position in scene coordinates.
     */
    QPointF endPos() const;

    /** Returns a path for the connection.
     */
    QPainterPath path(bool only_bezier=false) const;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    /** While the connection is open-ended, check for target ports.
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    /** On mouse release, connect to an available port if not already connected.
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    /** Check for mouse hover.
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    Datum* source;
    Datum* target;
    QPointF drag_pos;

    enum { NONE, VALID, INVALID, CONNECTED } drag_state;

    QPointF snap_pos;
    bool has_snap_pos;
    bool snapping;

    InputPort* target_port;
    bool hover;
};

#endif // CONNECTION_H
