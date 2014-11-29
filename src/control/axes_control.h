#ifndef AXES_CONTROL_H
#define AXES_CONTROL_H

#include "control/control.h"

class Viewport;

class AxesControl : public DummyControl
{
public:
    explicit AxesControl(Viewport* viewport);

    /** Return the bounding rectangle in screen coordinates.
     */
    QRectF boundingRect() const override;

    /** Override the custom bounds function with nothing.
     */
    QRectF bounds() const override { return QRectF(); }

    /** Paint the axes.
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
};

#endif // AXES_CONTROL_H
