#ifndef CYLINDER_CONTROL_H
#define CYLINDER_CONTROL_H

#include "control/control.h"
#include "control/wireframe.h"

class _CylinderRadiusControl : public WireframeControl
{
public:
    explicit _CylinderRadiusControl(Canvas* canvas, Node* node, QGraphicsItem* parent);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
};

class _CylinderSpanControl : public WireframeControl
{
public:
    explicit _CylinderSpanControl(Canvas* canvas, Node* node, QGraphicsItem* parent);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
};

class CylinderControl : public WireframeControl
{
public:
    explicit CylinderControl(Canvas* canvas, Node* node);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
protected:
    _CylinderRadiusControl* radius;
    _CylinderSpanControl* span;
};

#endif
