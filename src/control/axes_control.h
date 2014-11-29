#ifndef AXES_CONTROL_H
#define AXES_CONTROL_H

#include "control/dummy.h"

class Viewport;

class AxesControl : public DummyControl
{
public:
    explicit AxesControl(Viewport* viewport);

    /** Override the custom bounds function with nothing.
     */
    QRectF bounds(QMatrix4x4 m) const override;

    /** Paint the axes.
     */
    void paint(QMatrix4x4 m, bool highlight, QPainter *painter) override;
};

#endif // AXES_CONTROL_H
