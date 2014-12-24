#include <Python.h>
#include "control/transform/recenter_control.h"

RecenterControl::RecenterControl(Node* node, QObject* parent)
    : WireframeControl(node, parent)
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
