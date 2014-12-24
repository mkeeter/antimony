#include <Python.h>

#include <QPainter>

#include "graph/node/node.h"
#include "control/3d/point3d_control.h"

Point3DControl::Point3DControl(Node* node, QObject* parent, QString suffix)
    : WireframeControl(node, parent), suffix(suffix)
{
    watchDatums({"x" + suffix, "y" + suffix, "z" + suffix});
}

QVector3D Point3DControl::position() const
{
    return QVector3D(getValue("x" + suffix),
                     getValue("y" + suffix),
                     getValue("z" + suffix));
}


QVector<QPair<QVector3D, float>> Point3DControl::points() const
{
    return {{position(), 5}};
}

void Point3DControl::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(center);
    dragValue("x" + suffix, delta.x());
    dragValue("y" + suffix, delta.y());
    dragValue("z" + suffix, delta.z());
}

