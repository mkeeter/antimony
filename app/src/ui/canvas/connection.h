#ifndef CONNECTION_H
#define CONNECTION_H

#include <QGraphicsObject>
#include <QPointer>

#include "graph/watchers.h"

class InputPort;
class OutputPort;

class GraphScene;

class Connection : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Connection(OutputPort* source);
    explicit Connection(OutputPort* source, InputPort* target);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void setDragPos(QPointF p) { drag_pos = p; }

    OutputPort* getSource() const { return source; }
    InputPort* getTarget() const { return target; }

public slots:
    void onPortsMoved();
    void onHiddenChanged();

signals:
    void changed();

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

    bool isHidden() const;

    /** Checks to see whether we're on a valid port
     *  (and adjust drag_state accordingly).
     */
    void checkDragTarget();

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

    OutputPort* source;
    InputPort* target;
    const QColor color;

    QPointF drag_pos;

    enum { NONE, VALID, INVALID, CONNECTED } drag_state;

    QPointF start_pos;
    QPointF end_pos;

    QPointF snap_pos;
    bool has_snap_pos;
    bool snapping;

    bool hover;

    friend class InputPort;
};

#endif // CONNECTION_H
