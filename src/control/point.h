#ifndef CONTROL_POINT_H
#define CONTROL_POINT_H

#include <Python.h>

#include "control/control.h"

class ControlPoint : public Control
{
public:
    ControlPoint(Node* node, PyObject* drag_func=NULL);

    /*
     *  Updates the node, calling redraw if anything changed.
     */
    void update(float x, float y, float z, float r, QColor color,
                bool relative, PyObject* drag_func);

    /*
     *  Custom functions for this type of Control
     */
    QPainterPath shape(QMatrix4x4 m) const override;
    void paint(QMatrix4x4 m, bool highlight, QPainter* painter) override;

    float getR() const { return r; }
    QColor getColor() const { return color; }

    QVector3D pos() const override { return QVector3D(x, y, z); }

protected:
    float x, y, z, r;
    QColor color;
};

#endif
