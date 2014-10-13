#include <Python.h>
#include "control/transform/reflectx_control.h"

#include "ui/canvas.h"

ReflectXControl::ReflectXControl(Canvas* canvas, Node* node)
    : WireframeControl(canvas, node)
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

QPointF ReflectXControl::inspectorPosition() const
{
    return canvas->worldToScene(position());
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

