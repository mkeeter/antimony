#include <Python.h>

#include "control/3d/cube_control.h"
#include "control/3d/point3d_control.h"

#include "graph/node/node.h"

CubeControl::CubeControl(Node* node, QObject* parent)
    : WireframeControl(node, parent),
      a(new Point3DControl(node, this, "min")),
      b(new Point3DControl(node, this, "max"))
{
    watchDatums({"xmin","ymin","zmin","xmax","ymax","zmax"});
}

QVector<QVector<QVector3D>> CubeControl::lines() const
{
    float ax = getValue("xmin");
    float ay = getValue("ymin");
    float az = getValue("zmin");
    float bx = getValue("xmax");
    float by = getValue("ymax");
    float bz = getValue("zmax");

    return {
        {QVector3D(ax, ay, az), QVector3D(ax, by, az),
         QVector3D(bx, by, az), QVector3D(bx, ay, az), QVector3D(ax, ay, az)},
        {QVector3D(ax, ay, bz), QVector3D(ax, by, bz),
         QVector3D(bx, by, bz), QVector3D(bx, ay, bz), QVector3D(ax, ay, bz)},
        {QVector3D(ax, ay, az), QVector3D(ax, ay, bz)},
        {QVector3D(bx, ay, az), QVector3D(bx, ay, bz)},
        {QVector3D(ax, by, az), QVector3D(ax, by, bz)},
        {QVector3D(bx, by, az), QVector3D(bx, by, bz)}};
}

void CubeControl::drag(QVector3D center, QVector3D delta)
{
    a->drag(center, delta);
    b->drag(center, delta);
}

