#ifndef CIRCLE_CONTROL_H
#define CIRCLE_CONTROL_H

#include <Python.h>

#include <QObject>
#include <QVector3D>

#include "control/control.h"
#include "control/dummy.h"
#include "control/wireframe.h"
#include "control/2d/point2d_control.h"

class _RadiusControl : public WireframeControl
{
public:
    explicit _RadiusControl(Node* node, QObject* parent=0);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
    QVector3D center() const;
};

////////////////////////////////////////////////////////////////////////////////

class CircleControl : public DummyControl
{
public:
    explicit CircleControl(Node* node, QObject* parent=0);

    /** Drags the circle in x and y.
     */
    void drag(QVector3D center, QVector3D delta) override;

protected:
    QRectF bounds(QMatrix4x4 m, QMatrix4x4 t) const override;

    _RadiusControl* radius;
    Point2DControl* center;
};

#endif // CIRCLE_CONTROL_H
