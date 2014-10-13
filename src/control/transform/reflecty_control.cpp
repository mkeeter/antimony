#include <Python.h>
#include "control/transform/reflecty_control.h"

#include "ui/canvas.h"

ReflectYControl::ReflectYControl(Canvas* canvas, Node* node)
    : WireframeControl(canvas, node)
{
    watchDatums({"_x","y","_z"});
}

void ReflectYControl::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    dragValue("_x", d.x());
    dragValue("y", d.y());
    dragValue("_z", d.z());
}

QVector3D ReflectYControl::position() const
{
    return QVector3D(getValue("_x"), getValue("y"), getValue("_z"));
}

QPointF ReflectYControl::inspectorPosition() const
{
    return canvas->worldToScene(position());
}

QVector<QVector<QVector3D>> ReflectYControl::lines() const
{
    QVector3D p = position();
    return {{p + QVector3D(-1, 0, -1),
             p + QVector3D(-1, 0, 1),
             p + QVector3D(1, 0, 1),
             p + QVector3D(1, 0, -1),
             p + QVector3D(-1, 0, -1)}};
}

