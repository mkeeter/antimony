#include <Python.h>

#include "node/3d/point3d.h"
#include "node/manager.h"

#include "datum/float.h"

Point3D::Point3D(QString name, QString x, QString y, QString z,
                 QObject* parent)
    : Node(name, parent)
{
    new FloatDatum("x", x, this);
    new FloatDatum("y", y, this);
    new FloatDatum("z", z, this);
}

Point3D::Point3D(float x, float y, float z, float scale,
                 QObject *parent)
    : Node(NodeManager::manager()->getName("p"), parent)
{
    new FloatDatum("x", QString::number(x), this);
    new FloatDatum("y", QString::number(y), this);
    new FloatDatum("z", QString::number(z), this);
    Q_UNUSED(scale);
}
