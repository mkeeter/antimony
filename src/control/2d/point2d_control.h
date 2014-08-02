#ifndef POINT2D_CONTROL_H
#define POINT2D_CONTROL_H

#include <Python.h>

#include <QObject>
#include <QVector3D>

#include "control/control.h"

class Point2DControl : public Control
{
public:
    explicit Point2DControl(Canvas* canvas, Node* node,
                            QGraphicsItem* parent=0);

    /** Returns the bounding rect in screen coordinates.
     */
    QRectF bounds() const override;

    /** Paints a single circle.
     */
    void paintControl(QPainter *painter) override;

    /** Returns a painter path shape for selection.
     */
    QPainterPath shape() const override;

    /** Drags the point in x, y, and z.
     */
    void drag(QVector3D center, QVector3D delta) override;

    QPointF inspectorPosition() const override;

protected:
    QVector3D position() const;
};

#endif // POINT2D_CONTROL_H
