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

    /*
     *  Custom functions for this type of Control
     */
    QPainterPath shape(QMatrix4x4 m) const override;
    void paint(QMatrix4x4 m, bool highlight, QPainter* painter) override;

protected:
    float x, y, z, r;
    QColor color;
};

#endif
