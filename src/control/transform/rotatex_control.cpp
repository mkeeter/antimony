#include <Python.h>
#include "control/transform/rotatex_control.h"

#include "ui/canvas.h"

RotateXHandle::RotateXHandle(Canvas* canvas, Node* node,
                             QGraphicsItem* parent)
    : WireframeControl(canvas, node, parent)
{
    watchDatums({"_x","y","z","a"});
}

void RotateXHandle::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(delta);
    QVector3D d = center - position();
    setValue("a", atan2(d.z(), d.y()) * 180 / M_PI);
}

QVector<QPair<QVector3D, float>> RotateXHandle::points() const
{
    float angle = getValue("a") * M_PI / 180;
    return {{position() + QVector3D(0, cos(angle), sin(angle)), 5}};
}

QVector3D RotateXHandle::position() const
{
    return QVector3D(getValue("_x"), getValue("y"), getValue("z"));
}
////////////////////////////////////////////////////////////////////////////////

RotateXControl::RotateXControl(Canvas* canvas, Node* node)
    : WireframeControl(canvas, node),
      handle(new RotateXHandle(canvas, node, this))
{
    watchDatums({"_x","y","z","a"});
}

void RotateXControl::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    dragValue("_x", d.x());
    dragValue("y", d.y());
    dragValue("z", d.z());
}

QVector3D RotateXControl::position() const
{
    return QVector3D(getValue("_x"), getValue("y"), getValue("z"));
}

QPointF RotateXControl::inspectorPosition() const
{
    return canvas->worldToScene(position());
}

QVector<QVector<QVector3D>> RotateXControl::lines() const
{
    QVector3D p = position();
    float angle = fmod(getValue("a"), 360);
    while (angle < 0)   angle += 360;

    QVector<QVector3D> line =
        {p + QVector3D(0, cos(angle * M_PI / 180), sin(angle * M_PI / 180)),
         p, p + QVector3D(0, 1, 0)};

    int count = angle / 10;
    float step = angle / count;

    QVector<QVector3D> arc;
    for (int i=0; i <= count; ++i)
    {
        arc << p + 0.3*QVector3D(0, cos(step * i * M_PI / 180),
                                    sin(step * i * M_PI/180));
    }
    return {line, arc};
}

QVector<QPair<QVector3D, float>> RotateXControl::points() const
{
    return {{position(), 7}};
}
