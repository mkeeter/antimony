#include <Python.h>
#include "control/deform/scaley_control.h"

_ScaleYHandle::_ScaleYHandle(Node* node, bool pos, QObject* parent)
    : WireframeControl(node, parent), positive(pos)
{
    watchDatums({"_x","y","_z","s"});
}

QVector<QVector<QVector3D>> _ScaleYHandle::lines() const
{
    QVector3D p(getValue("_x"), getValue("y"), getValue("_z"));
    float s = getValue("s");

    if (positive)
    {
        return {{p + QVector3D(s*0.1, s*0.9, 0),
                 p + QVector3D(0, s, 0),
                 p + QVector3D(-s*0.1, s*0.9, 0)}};
    } else {
        return {{p - QVector3D(s*0.1, s*0.9, 0),
                 p - QVector3D(0, s, 0),
                 p - QVector3D(-s*0.1, s*0.9, 0)}};
    }
}

void _ScaleYHandle::drag(QVector3D center, QVector3D delta)
{
    dragValue("s", positive ? delta.y() : -delta.y());
    Q_UNUSED(center);
}
////////////////////////////////////////////////////////////////////////////////

ScaleYControl::ScaleYControl(Node* node, QObject* parent)
    : WireframeControl(node, parent),
      positive_handle(new _ScaleYHandle(node, true, this)),
      negative_handle(new _ScaleYHandle(node, false, this))
{
    watchDatums({"_x","y","_z","s"});
}

void ScaleYControl::drag(QVector3D c, QVector3D d)
{
    dragValue("_x", d.x());
    dragValue("y", d.y());
    dragValue("_z", d.z());
    Q_UNUSED(c);
}

QVector3D ScaleYControl::position() const
{
    return QVector3D(getValue("_x"), getValue("y"), getValue("_z"));
}

QVector<QVector<QVector3D>> ScaleYControl::lines() const
{
    QVector3D center = position();
    float s = getValue("s");

    return {{center - QVector3D(0, s, 0),
             center + QVector3D(0, s, 0)}};
}

QVector<QPair<QVector3D, float>> ScaleYControl::points() const
{
    return {{position(), 5}};
}
