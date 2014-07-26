#include <Python.h>

#include "node/3d/point3d_node.h"
#include "node/manager.h"

#include "datum/float_datum.h"

Point3D::Point3D(QObject* parent)
    : Node(parent)
{
    // Nothing to do here
}

Point3D::Point3D(QString name, QObject* parent)
    : Node(name, parent)
{
    // Nothing to do here
}

Point3D::Point3D(QString name, QString x, QString y, QString z,
                 QObject* parent)
    : Point3D(name, parent)
{
    new FloatDatum("x", x, this);
    new FloatDatum("y", y, this);
    new FloatDatum("z", z, this);
}

Point3D::Point3D(float x, float y, float z, float scale,
                 QObject *parent)
    : Point3D(NodeManager::manager()->getName("p"),
              QString::number(x),
              QString::number(y),
              QString::number(z), parent)
{
    Q_UNUSED(scale);
}
