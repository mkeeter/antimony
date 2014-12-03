#ifndef CYLINDER_CONTROL_H
#define CYLINDER_CONTROL_H

#include "control/control.h"
#include "control/wireframe.h"

class _CylinderRadiusControl : public WireframeControl
{
public:
    explicit _CylinderRadiusControl(Node* node, QObject* parent);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
};

class _CylinderSpanControl : public WireframeControl
{
public:
    explicit _CylinderSpanControl(Node* node, QObject* parent);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
};

class CylinderControl : public WireframeControl
{
public:
    explicit CylinderControl(Node* node, QObject* parent=NULL);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
protected:
    _CylinderRadiusControl* radius;
    _CylinderSpanControl* span;
};

#endif
