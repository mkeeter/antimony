#ifndef CONE_CONTROL_H
#define CONE_CONTROL_H

#include "control/control.h"
#include "control/wireframe.h"

#include "control/3d/cylinder_control.h"

class _ConeRadiusControl : public _CylinderRadiusControl
{
public:
    explicit _ConeRadiusControl(Canvas* canvas, Node* node,
                                QGraphicsItem* parent);
    QVector<QVector<QVector3D>> lines() const override;
};

class ConeControl : public CylinderControl
{
public:
    explicit ConeControl(Canvas* canvas, Node* node);
};

#endif
