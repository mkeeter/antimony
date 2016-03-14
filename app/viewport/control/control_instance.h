#pragma once

#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QPointer>

class Control;
class ViewportView;

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
     * Paint this object by calling the control's paint function.
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

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

    /*
     *  On backspace or delete, pass a deletion request up to the proxy
     */
    void keyPressEvent(QKeyEvent* event) override;

public slots:
    /*
     *  Indicate that the control should be redrawn
     */
    void redraw();

signals:
    /*
     *  Emitted when hover changes
     */
    void onFocus(bool focus);

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
