#include <Python.h>
#include "control/transform/reflectx_control.h"

ReflectXControl::ReflectXControl(Node* node, QObject* parent)
    : WireframeControl(node, parent)
{
    watchDatums({"x","_y","_z"});
}

void ReflectXControl::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    dragValue("x", d.x());
    dragValue("_y", d.y());
    dragValue("_z", d.z());
}

QVector3D ReflectXControl::position() const
{
    return QVector3D(getValue("x"), getValue("_y"), getValue("_z"));
}

QVector<QVector<QVector3D>> ReflectXControl::lines() const
{
    QVector3D p = position();
    return {{p + QVector3D(0, -1, -1),
             p + QVector3D(0, -1, 1),
             p + QVector3D(0, 1, 1),
             p + QVector3D(0, 1, -1),
             p + QVector3D(0, -1, -1)}};
}

