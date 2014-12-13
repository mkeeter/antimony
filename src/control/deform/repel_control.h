#ifndef REPEL_CONTROL_H
#define REPEL_CONTROL_H

#include "control/wireframe.h"
#include "control/3d/sphere_control.h"

#include "ui/util/colors.h"

class _RepelRadiusControl : public _SphereRadiusControl
{
public:
    explicit _RepelRadiusControl(Node* node, QObject* parent)
        : _SphereRadiusControl(node, parent) {}
protected:
    QColor defaultPenColor() const override { return Colors::blue; }
};

////////////////////////////////////////////////////////////////////////////////

class RepelControl : public WireframeControl
{
public:
    explicit RepelControl(Node* node, QObject* parent=NULL);

protected:
    void drag(QVector3D center, QVector3D delta) override;
    QVector<QVector<QVector3D>> lines() const override;

    QColor defaultPenColor() const override { return Colors::blue; }
    _RepelRadiusControl* radius;
};

#endif
