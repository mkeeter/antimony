#ifndef CONTROL_H
#define CONTROL_H

#include <QGraphicsObject>

// Forward declarations
class Canvas;
class Node;
class NodeViewer;

class Control : public QGraphicsObject
{
    Q_OBJECT
public:
    /** A control is a UI representation of a Node.
     *
     *  canvas is the Canvas object on which to draw.
     *  node is the target Node (or None in special cases)
     *  parent is a parent Control (as nested controls are allowed)
     */
    explicit Control(Canvas* canvas, Node* node, QGraphicsItem* parent=0);

    /** Finds a bounding box for a set of points in world coordinates.
     *
     *  The returned bounding box is in scene coordinates,
     *  as is the padding argument.
     */
    QRectF boundingBox(QVector<QVector3D> points, int padding=10) const;

protected:
    /** On hover enter, set _hover to true and update.
     */
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    /** On hover leave, set _hover to false and update.
     */
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    /** On double click, open a node viewer.
     */
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    /** Handle mouse clicks by preparing to drag.
     */
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    /** On mouse release (without drag), select object.
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    /** On mouse drag, call the virtual function drag.
     */
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    /** Called to drag the node around with the mouse.
     */
    virtual void drag(QVector3D center, QVector3D delta)=0;

    Canvas* canvas;
    Node* node;
    NodeViewer* viewer;

    bool _hover;
    bool _dragged;
    QPointF _click_pos;
};

#endif // CONTROL_H
