#ifndef SPHERE_CONTROL_H
#define SPHERE_CONTROL_H

#include "control/control.h"
#include "control/multiline.h"

class Point3DControl;

class SphereRadiusControl : public MultiLineControl
{
    Q_OBJECT
public:
    explicit SphereRadiusControl(Canvas* canvas, Node* node, QGraphicsItem* parent);
    virtual QVector<QVector<QVector3D>> lines() const override;
    virtual void drag(QVector3D center, QVector3D delta) override;
    QVector3D center() const;
};

////////////////////////////////////////////////////////////////////////////////

class SphereControl : public DummyControl
{
public:
    explicit SphereControl(Canvas* canvas, Node* node);

    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
    QRectF bounds() const override;

    SphereRadiusControl* radius;
    Point3DControl* center;
};

#endif
