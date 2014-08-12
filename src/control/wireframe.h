#ifndef WIREFRAME_H
#define WIREFRAME_H

#include "control/control.h"

class WireframeControl : public Control
{
public:
    explicit WireframeControl(Canvas* canvas, Node* node,
                              QGraphicsItem* parent=0);

    /** The selection region for this shape is given by its line and points.
     */
    QPainterPath shape() const override;

    /** The bounds are simply the bounds of the combined painter paths.
     */
    QRectF bounds() const override;

protected:

    void paintControl(QPainter* painter) override;

    /** Returns a QPainterPath that draws this shape's lines.
     */
    QPainterPath linePath() const;

    /** Returns a QPainterPath that draws this shape's circles.
     */
    QPainterPath pointPath() const;

    /** Returns a list of lines to draw.
     *  (must be overloaded in derived classes).
     */
    virtual QVector<QVector<QVector3D>> lines() const
        { return {}; }

    /** Provides a list of points to draw as point, radius pairs.
     *  (must be overloaded in derived classes).
     */
    virtual QVector<QPair<QVector3D, float>> points() const
        { return {}; }
};

#endif // WIREFRAME_H
