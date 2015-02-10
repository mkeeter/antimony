#ifndef CONTROL_POINT_H
#define CONTROL_POINT_H

#include "control/control.h"

class ControlPoint : public Control
{
public:
    ControlPoint(Node* node);

    /*
     *  Updates the node, calling redraw if anything changed.
     */
    void update(float x, float y, float z, float r, QColor color);

protected:
    float x, y, z, r;
    QColor color;
};

#endif
