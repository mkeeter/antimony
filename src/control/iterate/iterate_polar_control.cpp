#include <cmath>
#include "control/iterate/iterate_polar_control.h"

IteratePolarControl::IteratePolarControl(Node* node, QObject* parent)
    : WireframeControl(node, parent)
{
    watchDatums({"x", "y"});
}

void IteratePolarControl::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(center);
    dragValue("x", delta.x());
    dragValue("y", delta.y());
}

QVector<QVector<QVector3D>> IteratePolarControl::lines(
        QMatrix4x4 m, QMatrix4x4 t) const
{
    Q_UNUSED(t);

    QVector3D p = position();
    float scale = (m * QVector3D(1, 0, 0) - m*QVector3D(0, 0, 0)).length();
    float r = 15 / scale;
    int n = 64;

    QVector<QVector3D> circle;
    for (int i=0; i <= n; ++i)
        circle << p + r*QVector3D(cos(i*2*M_PI/n), sin(i*2*M_PI/n), 0);

    return {circle};
}


QVector<QPair<QVector3D, float>> IteratePolarControl::points() const
{
    return {{position(), 5}};
}

QVector3D IteratePolarControl::position() const
{
    return QVector3D(getValue("x"), getValue("y"), 0);
}
