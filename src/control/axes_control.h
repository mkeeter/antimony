#ifndef AXES_CONTROL_H
#define AXES_CONTROL_H

#include "control/control.h"

class Canvas;

class AxesControl : public DummyControl
{
    Q_OBJECT
public:
    explicit AxesControl(Canvas* canvas);

    /** Return the bounding rectangle in screen coordinates.
     */
    virtual QRectF boundingRect() const override;

    /** Override the custom bounds function with nothing.
     */
    virtual QRectF bounds() const override { return QRectF(); }

    /** Paint the axes.
     */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;
};

#endif // AXES_CONTROL_H
