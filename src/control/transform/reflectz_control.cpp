#include <Python.h>
#include "control/transform/reflectz_control.h"

#include "ui/canvas.h"

ReflectZControl::ReflectZControl(Canvas* canvas, Node* node)
    : WireframeControl(canvas, node)
{
    watchDatums({"_x","_y","z"});
}

void ReflectZControl::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    dragValue("_x", d.x());
    dragValue("_y", d.y());
    dragValue("z", d.z());
}

QVector3D ReflectZControl::position() const
{
    return QVector3D(getValue("_x"), getValue("_y"), getValue("z"));
}

QPointF ReflectZControl::inspectorPosition() const
{
    return canvas->worldToScene(position());
}

QVector<QVector<QVector3D>> ReflectZControl::lines() const
{
    QVector3D p = position();
    return {{p + QVector3D(-1, -1, 0),
             p + QVector3D(-1, 1, 0),
             p + QVector3D(1, 1, 0),
             p + QVector3D(1, -1, 0),
             p + QVector3D(-1, -1, 0)}};
}


