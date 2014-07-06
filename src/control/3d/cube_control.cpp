#include <Python.h>

#include "control/3d/cube_control.h"
#include "control/3d/point3d_control.h"

#include "node/node.h"
#include "ui/canvas.h"

CubeControl::CubeControl(Canvas* canvas, Node* node)
    : MultiLineControl(canvas, node),
      a(new Point3DControl(canvas, node->findChild<Node*>("a"), this)),
      b(new Point3DControl(canvas, node->findChild<Node*>("b"), this))
{
    watchDatums({"a.x","a.y","a.z","b.x","b.y","b.z"});
}

QVector<QVector<QVector3D>> CubeControl::lines() const
{
    float ax = a->getValue("x");
    float ay = a->getValue("y");
    float az = a->getValue("z");
    float bx = b->getValue("x");
    float by = b->getValue("y");
    float bz = b->getValue("z");

    return {
        {QVector3D(ax, ay, az), QVector3D(ax, by, az),
         QVector3D(bx, by, az), QVector3D(bx, ay, az), QVector3D(ax, ay, az)},
        {QVector3D(ax, ay, bz), QVector3D(ax, by, bz),
         QVector3D(bx, by, bz), QVector3D(bx, ay, bz), QVector3D(ax, ay, bz)}};
}

void CubeControl::drag(QVector3D center, QVector3D delta)
{
    a->drag(center, delta);
    b->drag(center, delta);
}

QPointF CubeControl::inspectorPosition() const
{
    return b->inspectorPosition();
}
