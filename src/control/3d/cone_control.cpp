#include <Python.h>

#include <QVector3D>

#include "control/3d/cone_control.h"
#include "ui/canvas.h"

_ConeRadiusControl::_ConeRadiusControl(Canvas* canvas, Node* node,
                                       QGraphicsItem* parent)
    : _CylinderRadiusControl(canvas, node, parent)
{
    // Nothing to do here
}

QVector<QVector<QVector3D>> _ConeRadiusControl::lines() const
{
    return {_CylinderRadiusControl::lines()[1]};
}

ConeControl::ConeControl(Canvas* canvas, Node* node)
    : CylinderControl(canvas, node)
{
    radius->deleteLater();
    radius = new _ConeRadiusControl(canvas, node, this);
}
