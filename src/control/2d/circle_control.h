#ifndef CIRCLE_CONTROL_H
#define CIRCLE_CONTROL_H

#include <Python.h>

#include <QObject>
#include <QVector3D>

#include "control/control.h"
#include "control/wireframe.h"
#include "control/2d/point2d_control.h"

class _RadiusControl : public WireframeControl
{
public:
    explicit _RadiusControl(Canvas* canvas, Node* node, QGraphicsItem* parent);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
    QVector3D center() const;
};

////////////////////////////////////////////////////////////////////////////////

class CircleControl : public DummyControl
{
public:
    explicit CircleControl(Canvas* canvas, Node* node);

    /** Drags the circle in x and y.
     */
    void drag(QVector3D center, QVector3D delta) override;

protected:
    QPointF baseInputPosition() const override;
    QPointF baseOutputPosition() const override;

    /** Returns a position for the inspector.
     */
    QPointF inspectorPosition() const override;

    QRectF bounds() const override;
    QVector3D position() const;

    _RadiusControl* radius;
    Point2DControl* center;
};

#endif // CIRCLE_CONTROL_H
