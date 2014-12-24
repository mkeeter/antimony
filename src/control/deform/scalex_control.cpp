#include <Python.h>
#include "control/deform/scalex_control.h"

_ScaleXHandle::_ScaleXHandle(Node* node, bool pos, QObject* parent)
    : WireframeControl(node, parent), positive(pos)
{
    watchDatums({"x","_y","_z","s"});
}

QVector<QVector<QVector3D>> _ScaleXHandle::lines() const
{
    QVector3D p(getValue("x"), getValue("_y"), getValue("_z"));
    float s = getValue("s");

    if (positive)
    {
        return {{p + QVector3D(s*0.9, s*0.1, 0),
                 p + QVector3D(s, 0, 0),
                 p + QVector3D(s*0.9, -s*0.1, 0)}};
    } else {
        return {{p - QVector3D(s*0.9, s*0.1, 0),
                 p - QVector3D(s, 0, 0),
                 p - QVector3D(s*0.9, -s*0.1, 0)}};
    }
}

void _ScaleXHandle::drag(QVector3D center, QVector3D delta)
{
    dragValue("s", positive ? delta.x() : -delta.x());
    Q_UNUSED(center);
}
////////////////////////////////////////////////////////////////////////////////

ScaleXControl::ScaleXControl(Node* node, QObject* parent)
    : WireframeControl(node, parent),
      positive_handle(new _ScaleXHandle(node, true, this)),
      negative_handle(new _ScaleXHandle(node, false, this))
{
    watchDatums({"x","_y","_z","s"});
}

void ScaleXControl::drag(QVector3D c, QVector3D d)
{
    dragValue("x", d.x());
    dragValue("_y", d.y());
    dragValue("_z", d.z());
    Q_UNUSED(c);
}

QVector3D ScaleXControl::position() const
{
    return QVector3D(getValue("x"), getValue("_y"), getValue("_z"));
}
QVector<QVector<QVector3D>> ScaleXControl::lines() const
{
    QVector3D center = position();
    float s = getValue("s");

    return {{center - QVector3D(s, 0, 0),
             center + QVector3D(s, 0, 0)}};
}

QVector<QPair<QVector3D, float>> ScaleXControl::points() const
{
    return {{position(), 5}};
}
