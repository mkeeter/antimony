#include <Python.h>
#include "control/transform/recenter_control.h"

#include "ui/canvas.h"

RecenterControl::RecenterControl(Canvas* canvas, Node* node)
    : WireframeControl(canvas, node)
{
    watchDatums({"x", "y", "z"});
}

void RecenterControl::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    dragValue("x", d.x());
    dragValue("y", d.y());
    dragValue("z", d.z());
}

QVector3D RecenterControl::position() const
{
    return QVector3D(getValue("x"), getValue("y"), getValue("z"));
}

QPointF RecenterControl::inspectorPosition() const
{
    return canvas->worldToScene(position());
}

QVector<QVector<QVector3D>> RecenterControl::lines() const
{
    QVector3D p = position();
    return {{p + QVector3D(-0.3, 0, 0), p + QVector3D(0.3, 0, 0)},
            {p + QVector3D(0, -0.3, 0), p + QVector3D(0, 0.3, 0)},
            {p + QVector3D(0, 0, -0.3), p + QVector3D(0, 0, 0.3)}};
}

QVector<QPair<QVector3D, float>> RecenterControl::points() const
{
    return {{position(), 5}};
}
