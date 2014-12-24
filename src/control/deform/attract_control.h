#ifndef ATTRACT_CONTROL_H
#define ATTRACT_CONTROL_H

#include "control/wireframe.h"
#include "control/3d/sphere_control.h"

#include "ui/util/colors.h"

class _AttractRadiusControl : public _SphereRadiusControl
{
public:
    explicit _AttractRadiusControl(Node* node, QObject* parent)
        : _SphereRadiusControl(node, parent) {}
protected:
    QColor defaultPenColor() const override { return Colors::blue; }
};

////////////////////////////////////////////////////////////////////////////////

class AttractControl : public WireframeControl
{
public:
    explicit AttractControl(Node* node, QObject* parent=NULL);

protected:
    void drag(QVector3D center, QVector3D delta) override;
    QVector<QVector<QVector3D>> lines() const override;

    QColor defaultPenColor() const override { return Colors::blue; }
    _AttractRadiusControl* radius;
};

#endif
