#ifndef SPHERE_CONTROL_H
#define SPHERE_CONTROL_H

#include "control/wireframe.h"
#include "control/dummy.h"

class Point3DControl;

class _SphereRadiusControl : public WireframeControl
{
public:
    explicit _SphereRadiusControl(Node* node, QObject* parent);
    QVector<QVector<QVector3D>> lines(QMatrix4x4 t) const override;
    void drag(QVector3D center, QVector3D delta) override;
    QVector3D center() const;
};

////////////////////////////////////////////////////////////////////////////////

class SphereControl : public DummyControl
{
public:
    explicit SphereControl(Node* node, QObject* parent=NULL);

protected:
    void drag(QVector3D center, QVector3D delta) override;

    _SphereRadiusControl* radius;
    Point3DControl* center;
};

#endif
