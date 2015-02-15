#ifndef CONTROL_WIREFRAME_H
#define CONTROL_WIREFRAME_H

#include <Python.h>

#include "control/control.h"

class ControlWireframe : public Control
{
public:
    ControlWireframe(Node* node);

    /*
     * Updates the control, calling redraw if anything changed.
     */
    void update(QVector<QVector3D> pts, float r, QColor color);

    QPainterPath shape(QMatrix4x4 m) const override;
    void paint(QMatrix4x4 m, bool highlight, QPainter* painter);

    float getT() const { return t; }
    QColor getColor() const { return color; }

protected:
    QVector<QVector3D> pts;
    float t;
    QColor color;
};

#endif
