#ifndef CONTROL_H
#define CONTROL_H

#include <QGraphicsObject>
#include <QPointer>

// Forward declarations
class Canvas;
class Node;
class NodeInspector;

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

    /** Returns this control's relevant node.
     */
    Node* getNode() { return node; }

    /** Returns the desired editor point (in scene coordinates)
     */
    virtual QPointF inspectorPosition() const { return QPointF(); }

    /** Returns the canvas object in which this control is drawn.
     */
    Canvas* getCanvas() { return canvas;}

protected slots:
    void redraw();

signals:
    void inspectorPositionChanged();

protected:

    /** Mark a set of datums as causing a re-render when changed.
     */
    void watchDatums(QVector<QString> datums);

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

    /** Gets the value of a specific datum (which must be a double).
     */
    double getValue(QString name) const;

    /** Attempts to drag a particular datum's value.
     */
    void dragValue(QString name, double delta);

    /** Sets the painter pen to a reasonable default value.
     */
    void setDefaultPen(QPainter* painter) const;

    /** Sets the painter brush to a reasonable value.
     */
    void setDefaultBrush(QPainter* painter) const;


    Canvas* canvas;
    Node* node;
    QPointer<NodeInspector> viewer;

    bool _hover;
    bool _dragged;
    QPointF _click_pos;
};


/** A DummyControl is non-interactive by default and below all other controls.
 */
class DummyControl : public Control
{
    Q_OBJECT
public:
    explicit DummyControl(Canvas* canvas, Node* node, QGraphicsItem* parent=0);
    virtual void drag(QVector3D center, QVector3D delta) override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
    virtual QPainterPath shape() const;
};

#endif // CONTROL_H
