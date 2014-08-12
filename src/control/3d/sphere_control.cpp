#include <Python.h>

#include "control/3d/sphere_control.h"
#include "control/3d/point3d_control.h"

#include "ui/canvas.h"

#include <cmath>

SphereRadiusControl::SphereRadiusControl(Canvas* canvas, Node* node, QGraphicsItem* parent)
    : WireframeControl(canvas, node, parent)
{
    watchDatums({"x","y","z","r"});
}

QVector<QVector<QVector3D>> SphereRadiusControl::lines() const
{
    QVector3D c = center();
    float r = getValue("r");

    QMatrix4x4 m;
    m.rotate(-canvas->getYaw()   * 180 / M_PI, QVector3D(0, 0, 1));
    m.rotate(-canvas->getPitch() * 180 / M_PI, QVector3D(1, 0, 0));

    int n = 64;
    QVector<QVector3D> circle;
    for (int i=0; i <= n; ++i)
    {
        circle << c + m * QVector3D(cos(i*2*M_PI/n), sin(i*2*M_PI/n), 0) * r;
    }
    return {circle};
}


QVector3D SphereRadiusControl::center() const
{
    return QVector3D(getValue("x"), getValue("y"), getValue("z"));
}

void SphereRadiusControl::drag(QVector3D c, QVector3D d)
{
    dragValue("r", QVector3D::dotProduct((c - center()).normalized(), d));
}

////////////////////////////////////////////////////////////////////////////////

SphereControl::SphereControl(Canvas* canvas, Node* node)
    : DummyControl(canvas, node),
      radius(new SphereRadiusControl(canvas, node, this)),
      center(new Point3DControl(canvas, node, this))
{
    watchDatums({"x","y","z","r"});
}


void SphereControl::drag(QVector3D c, QVector3D delta)
{
    center->drag(c, delta);
}

QPointF SphereControl::inspectorPosition() const
{
    return canvas->worldToScene(radius->center());
}

QPointF SphereControl::baseInputPosition() const
{
    return (bounds().bottomLeft() + bounds().topLeft()) / 2;
}

QPointF SphereControl::baseOutputPosition() const
{
    return (bounds().bottomRight() + bounds().topRight()) / 2;
}


QRectF SphereControl::bounds() const
{
    return radius->boundingRect();
}
