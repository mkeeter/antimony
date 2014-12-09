#ifndef WIREFRAME_H
#define WIREFRAME_H

#include "control/control.h"

class WireframeControl : public Control
{
public:
    explicit WireframeControl(Node* node, QObject* parent=0);

    /** The selection region for this shape is given by its line and points.
     */
    QPainterPath shape(QMatrix4x4 m, QMatrix4x4 t) const override;

    /** The bounds are simply the bounds of the combined painter paths.
     */
    QRectF bounds(QMatrix4x4 m, QMatrix4x4 t) const override;

protected:

    void paint(QMatrix4x4 m, QMatrix4x4 t,
               bool highlight, QPainter* painter) override;

    /** Returns a QPainterPath that draws this shape's lines.
     */
    QPainterPath linePath(QMatrix4x4 m, QMatrix4x4 t) const;

    /** Returns a QPainterPath that draws this shape's circles.
     */
    QPainterPath pointPath(QMatrix4x4 m, QMatrix4x4 t) const;

    /** Returns a list of lines to draw.
     *  (must be overloaded in derived classes).
     */
    virtual QVector<QVector<QVector3D>> lines() const
        { return {}; }
    virtual QVector<QVector<QVector3D>> lines(QMatrix4x4 t) const
        { Q_UNUSED(t); return lines(); }

    /** Provides a list of points to draw as point, radius pairs.
     *  (must be overloaded in derived classes).
     */
    virtual QVector<QPair<QVector3D, float>> points() const
        { return {}; }
    virtual QVector<QPair<QVector3D, float>> points(QMatrix4x4 t) const
        { Q_UNUSED(t); return points(); }
};

#endif // WIREFRAME_H
