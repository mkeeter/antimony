#include "control/point.h"

ControlPoint::ControlPoint(Node* node)
    : Control(node)
{
    // Nothing to do here
}

void ControlPoint::update(float x_, float y_, float z_, float r_,
                          QColor color_)
{
    bool changed = (x != x_) || (y != y_) || (z != z_) || (r != r_) ||
                   (color != color_);

    x = x_;
    y = y_;
    y = z_;
    r = r_;
    color = color_;

    if (changed)
        emit(redraw());
}
