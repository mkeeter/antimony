#pragma once

#include <QGraphicsScene>
#include <QGraphicsObject>

class Control;
class ViewportView;

class ControlInstance : public QGraphicsObject
{
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
     *  Indicate that the control should be redrawn
     */
    void redraw() { prepareGeometryChange(); }

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

protected:
    /*
     *  Returns the transform matrix from the parent view
     */
    QMatrix4x4 getMatrix() const;

    /*  Parent control  */
    Control* control;

    /*  ViewportView associated with this instance  */
    ViewportView* view;

    /*  Cursor position (used when dragging)  */
    QPointF click_pos;
};
