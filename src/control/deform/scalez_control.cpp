#include <Python.h>
#include "control/deform/scalez_control.h"

_ScaleZHandle::_ScaleZHandle(Node* node, bool pos, QObject* parent)
    : WireframeControl(node, parent), positive(pos)
{
    watchDatums({"_x","_y","z","s"});
}

QVector<QVector<QVector3D>> _ScaleZHandle::lines() const
{
    QVector3D p(getValue("_x"), getValue("_y"), getValue("z"));
    float s = getValue("s");

    if (positive)
    {
        return {{p + QVector3D(s*0.1, 0, s*0.9),
                 p + QVector3D(0, 0, s),
                 p + QVector3D(-s*0.1, 0, s*0.9)}};
    } else {
        return {{p - QVector3D(s*0.1, 0, s*0.9),
                 p - QVector3D(0, 0, s),
                 p - QVector3D(-s*0.1, 0, s*0.9)}};
    }
}

void _ScaleZHandle::drag(QVector3D center, QVector3D delta)
{
    dragValue("s", positive ? delta.z() : -delta.z());
    Q_UNUSED(center);
}
////////////////////////////////////////////////////////////////////////////////

ScaleZControl::ScaleZControl(Node* node, QObject* parent)
    : WireframeControl(node, parent),
      positive_handle(new _ScaleZHandle(node, true, this)),
      negative_handle(new _ScaleZHandle(node, false, this))
{
    watchDatums({"_x","_y","z","s"});
}

void ScaleZControl::drag(QVector3D c, QVector3D d)
{
    dragValue("_x", d.x());
    dragValue("_y", d.y());
    dragValue("z", d.z());
    Q_UNUSED(c);
}

QVector3D ScaleZControl::position() const
{
    return QVector3D(getValue("_x"), getValue("_y"), getValue("z"));
}

QVector<QVector<QVector3D>> ScaleZControl::lines() const
{
    QVector3D center = position();
    float s = getValue("s");

    return {{center - QVector3D(0, 0, s),
             center + QVector3D(0, 0, s)}};
}

QVector<QPair<QVector3D, float>> ScaleZControl::points() const
{
    return {{position(), 5}};
}
