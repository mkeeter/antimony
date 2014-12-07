#include <Python.h>

#include <QVector3D>

#include "control/3d/cone_control.h"

_ConeRadiusControl::_ConeRadiusControl(Node* node, QObject* parent)
    : _CylinderRadiusControl(node, parent)
{
    // Nothing to do here
}

QVector<QVector<QVector3D>> _ConeRadiusControl::lines() const
{
    return {_CylinderRadiusControl::lines()[1]};
}

ConeControl::ConeControl(Node* node, QObject* parent)
    : CylinderControl(node, parent)
{
    // We build the ConeControl on the CylinderControl; however, we
    // need to abandon the radius created by the Cylinder (otherwise
    // the system will try to make ControlProxies for it).
    radius->setParent(NULL);
    radius->deleteLater();

    radius = new _ConeRadiusControl(node, this);
}
