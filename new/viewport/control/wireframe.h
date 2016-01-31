#pragma once

#include <Python.h>

#include "viewport/control/control.h"

class WireframeControl : public Control
{
public:
    WireframeControl(NodeProxy* node);

    /*
     * Updates the control, calling redraw if anything changed.
     */
    void update(QVector<QVector3D> pts, float r, QColor color, bool close,
                bool relative, PyObject* drag_func);

    /*
     *  Custom functions for this type of Control
     */
    QPainterPath shape(QMatrix4x4 m) const override;
    void paint(QMatrix4x4 m, bool highlight, QPainter* painter) override;

    /*
     *  Returns the average position of the wireframe
     */
    QVector3D pos() const override;

protected:
    /*
     *  Returns a painter path that traces the wireframe's outline
     */
    QPainterPath path(QMatrix4x4 m) const;

    /*  Wireframe shape  */
    QVector<QVector3D> pts;

    /*  Line thickness  */
    float t;

    /*  Line color  */
    QColor color;

    /*  Flag indicating whether to close the contour  */
    bool close;

    friend struct ScriptUIHooks;
};
