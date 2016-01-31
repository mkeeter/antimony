#pragma once

#include <Python.h>

#include "viewport/control/control.h"

class PointControl : public Control
{
public:
    PointControl(NodeProxy* node);

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

    QVector3D pos() const override { return QVector3D(x, y, z); }

protected:
    /*  Position and radius  */
    float x, y, z, r;

    /*  Point color  */
    QColor color;

    friend struct ScriptUIHooks;
};
