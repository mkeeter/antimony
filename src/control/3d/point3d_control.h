#ifndef POINT3D_CONTROL_H
#define POINT3D_CONTROL_H

#include <Python.h>

#include <QObject>
#include <QVector3D>

#include "control/control.h"

class Point3DControl : public Control
{
public:
    explicit Point3DControl(Canvas* canvas, Node* node,
                            QGraphicsItem* parent=0);

    /** Returns the bounding rect in screen coordinates.
     */
    virtual QRectF bounds() const override;

    /** Paints a single circle.
     */
    virtual void paintControl(QPainter *painter) override;

    /** Returns a painter path shape for selection.
     */
    virtual QPainterPath shape() const override;

    /** Drags the point in x, y, and z.
     */
    virtual void drag(QVector3D center, QVector3D delta) override;

    virtual QPointF inspectorPosition() const override;

protected:
    QVector3D position() const;
};

#endif // POINT3D_CONTROL_H
