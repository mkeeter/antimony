#include <Python.h>

#include <QPainter>

#include "control/2d/point2d_control.h"

Point2DControl::Point2DControl(Node* node, QObject* parent, QString suffix)
    : WireframeControl(node, parent), suffix(suffix)
{
    watchDatums({"x" + suffix, "y" + suffix});
}

QVector<QPair<QVector3D, float>> Point2DControl::points() const
{
    return {{position(), 5}};
}

QVector3D Point2DControl::position() const
{
    return QVector3D(getValue("x" + suffix), getValue("y" + suffix), 0);
}

void Point2DControl::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(center);
    dragValue("x" + suffix, delta.x());
    dragValue("y" + suffix, delta.y());
}
