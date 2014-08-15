#include <Python.h>
#include "control/transform/rotatez_control.h"

#include "ui/canvas.h"

RotateZHandle::RotateZHandle(Canvas* canvas, Node* node,
                             QGraphicsItem* parent)
    : WireframeControl(canvas, node, parent)
{
    watchDatums({"x","y","_z","a"});
}

void RotateZHandle::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(delta);
    QVector3D d = center - position();
    setValue("a", atan2(d.y(), d.x()) * 180 / M_PI);
}

QVector<QPair<QVector3D, float>> RotateZHandle::points() const
{
    float angle = getValue("a") * M_PI / 180;
    return {{position() + QVector3D(cos(angle), sin(angle), 0), 5}};
}

QVector3D RotateZHandle::position() const
{
    return QVector3D(getValue("x"), getValue("y"), getValue("_z"));
}
////////////////////////////////////////////////////////////////////////////////

RotateZControl::RotateZControl(Canvas* canvas, Node* node)
    : WireframeControl(canvas, node),
      handle(new RotateZHandle(canvas, node, this))
{
    watchDatums({"x","y","_z","a"});
}

void RotateZControl::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    dragValue("x", d.x());
    dragValue("y", d.y());
    dragValue("_z", d.z());
}

QVector3D RotateZControl::position() const
{
    return QVector3D(getValue("x"), getValue("y"), getValue("_z"));
}

QPointF RotateZControl::inspectorPosition() const
{
    return canvas->worldToScene(position());
}

QVector<QVector<QVector3D>> RotateZControl::lines() const
{
    QVector3D p = position();
    float angle = fmod(getValue("a"), 360);
    while (angle < 0)   angle += 360;

    QVector<QVector3D> line =
        {p + QVector3D(cos(angle * M_PI / 180), sin(angle * M_PI / 180), 0),
         p, p + QVector3D(1, 0, 0)};

    int count = angle / 10;
    float step = angle / count;

    QVector<QVector3D> arc;
    for (int i=0; i <= count; ++i)
    {
        arc << p + 0.3*QVector3D(cos(step * i * M_PI / 180),
                                 sin(step * i * M_PI/180), 0);
    }
    return {line, arc};
}

QVector<QPair<QVector3D, float>> RotateZControl::points() const
{
    return {{position(), 7}};
}
