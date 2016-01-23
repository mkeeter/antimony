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
protected:
    /*
     *  Returns the transform matrix from the parent view
     */
    QMatrix4x4 getMatrix() const;

    Control* control;
    ViewportView* view;
};
