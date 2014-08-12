#ifndef SPHERE_CONTROL_H
#define SPHERE_CONTROL_H

#include "control/wireframe.h"

class Point3DControl;

class SphereRadiusControl : public WireframeControl
{
    Q_OBJECT
public:
    explicit SphereRadiusControl(Canvas* canvas, Node* node, QGraphicsItem* parent);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
    QVector3D center() const;
};

////////////////////////////////////////////////////////////////////////////////

class SphereControl : public DummyControl
{
public:
    explicit SphereControl(Canvas* canvas, Node* node);

protected:
    QPointF baseInputPosition() const override;
    QPointF baseOutputPosition() const override;

    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
    QRectF bounds() const override;

    SphereRadiusControl* radius;
    Point3DControl* center;
};

#endif
