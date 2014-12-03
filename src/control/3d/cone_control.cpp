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
    radius->deleteLater();
    radius = new _ConeRadiusControl(node, this);
}
