#include <Python.h>
#include "control/transform/rotatey_control.h"

#include "ui/canvas.h"

RotateYHandle::RotateYHandle(Canvas* canvas, Node* node,
                             QGraphicsItem* parent)
    : WireframeControl(canvas, node, parent)
{
    watchDatums({"x","_y","z","a"});
}

void RotateYHandle::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(delta);
    QVector3D d = center - position();
    setValue("a", atan2(d.z(), d.x()) * 180 / M_PI);
}

QVector<QPair<QVector3D, float>> RotateYHandle::points() const
{
    float angle = getValue("a") * M_PI / 180;
    return {{position() + QVector3D(cos(angle), 0, sin(angle)), 5}};
}

QVector3D RotateYHandle::position() const
{
    return QVector3D(getValue("x"), getValue("_y"), getValue("z"));
}
////////////////////////////////////////////////////////////////////////////////

RotateYControl::RotateYControl(Canvas* canvas, Node* node)
    : WireframeControl(canvas, node),
      handle(new RotateYHandle(canvas, node, this))
{
    watchDatums({"x","_y","z","a"});
}

void RotateYControl::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    dragValue("x", d.x());
    dragValue("_y", d.y());
    dragValue("z", d.z());
}

QVector3D RotateYControl::position() const
{
    return QVector3D(getValue("x"), getValue("_y"), getValue("z"));
}

QPointF RotateYControl::inspectorPosition() const
{
    return canvas->worldToScene(position());
}

QVector<QVector<QVector3D>> RotateYControl::lines() const
{
    QVector3D p = position();
    float angle = fmod(getValue("a"), 360);
    while (angle < 0)   angle += 360;

    QVector<QVector3D> line =
        {p + QVector3D(cos(angle * M_PI / 180), 0, sin(angle * M_PI / 180)),
         p, p + QVector3D(1, 0, 0)};

    int count = angle / 10;
    float step = angle / count;

    QVector<QVector3D> arc;
    for (int i=0; i <= count; ++i)
    {
        arc << p + 0.3*QVector3D(cos(step * i * M_PI / 180), 0,
                                 sin(step * i * M_PI/180));
    }
    return {line, arc};
}

QVector<QPair<QVector3D, float>> RotateYControl::points() const
{
    return {{position(), 7}};
}
