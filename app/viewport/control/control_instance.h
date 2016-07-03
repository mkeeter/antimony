#pragma once

#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QMatrix4x4>
#include <QPointer>

class Control;
class ViewportView;
class Node;

class ControlInstance : public QGraphicsObject
{
Q_OBJECT

public:
    ControlInstance(Control* c, ViewportView* view);

    /*
     *  Returns the bounding rect of the QGraphicsObject by calling
     *  Control::bounds.
     */
    QRectF boundingRect() const override;

    /*
     *  Returns a more precise shape for selection region.
     */
    QPainterPath shape() const override;

    /*
     *  Looks up this node's name
     */
    QString getName() const;

    /*
     *  Looks up the relevant node
     */
    const Node* getNode() const;

    /*
     *  Looks up the parent control
     */
    Control* getControl() const;

    /*
     * Paint this object by calling the control's paint function.
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    /*
     *  On mouse press, open a 'jump to node' menu
     */
    void openContextMenu();

    /*
     *  Handle mouse clicks by preparing to drag.
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    /*
     *  On mouse release, release mouse (if it was grabbed)
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    /*
     *  On mouse drag, call the parent Control's drag_func
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    /*
     *  On hover enter and exit, set hover flag and call update
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

public slots:
    /*
     *  Indicate that the control should be redrawn
     *  M is unused, present just for ease of connection.
     */
    void onViewChanged(QMatrix4x4 M=QMatrix4x4());
    void redraw();

    /*
     *  Deletes the associated node
     */
    void onDeleteNode();

signals:
    /*
     *  Emitted when hover changes
     */
    void onFocus(bool focus);

    /*
     *  Requests that we zoom to the given node
     */
    void onZoomTo();

protected:
    /*
     *  Returns the transform matrix from the parent view
     */
    QMatrix4x4 getMatrix() const;

    /*  Weak pointer to parent control  */
    QPointer<Control> control;

    /*  ViewportView associated with this instance  */
    ViewportView* view;

    /*  Cursor position (used when dragging)  */
    QPointF click_pos;

    /*  Flag indicating whether the mouse is over this point  */
    bool hover=false;
};
