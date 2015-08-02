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
    void update(QVector<QVector3D> pts, float r, QColor color, bool close,
                bool relative, PyObject* drag_func);

    QPainterPath shape(QMatrix4x4 m) const override;
    void paint(QMatrix4x4 m, bool highlight, QPainter* painter);

    float getT() const { return t; }
    QColor getColor() const { return color; }
    bool getClose() const { return close; }

    QVector3D pos() const override;;

protected:
    QPainterPath path(QMatrix4x4 m) const;

    QVector<QVector3D> pts;
    float t;
    QColor color;
    bool close;
};

#endif
