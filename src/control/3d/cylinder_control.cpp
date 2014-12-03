#include <Python.h>

#include <QVector3D>

#include "control/3d/cylinder_control.h"

_CylinderRadiusControl::_CylinderRadiusControl(Node* node, QObject* parent)
    : WireframeControl(node, parent)
{
    watchDatums({"x", "y", "z0", "z1", "r"});
}

QVector<QVector<QVector3D>> _CylinderRadiusControl::lines() const
{
    QVector3D a(getValue("x"), getValue("y"), getValue("z0"));
    QVector3D b(a.x(), a.y(), getValue("z1"));

    float r = getValue("r");
    int n = 64;

    QVector<QVector3D> lower;
    QVector<QVector3D> upper;
    for (int i=0; i <= n; ++i)
    {
        lower << a + r*QVector3D(cos(i*2*M_PI/n), sin(i*2*M_PI/n), 0);
        upper << b + r*QVector3D(cos(i*2*M_PI/n), sin(i*2*M_PI/n), 0);
    }

    return {upper, lower};
}

void _CylinderRadiusControl::drag(QVector3D c, QVector3D d)
{
    dragValue("r", QVector3D::dotProduct((c - QVector3D(
                        getValue("x"), getValue("y"), 0))
                .normalized(), d));
}

////////////////////////////////////////////////////////////////////////////////

_CylinderSpanControl::_CylinderSpanControl(Node* node, QObject* parent)
    : WireframeControl(node, parent)
{
    watchDatums({"x", "y", "z0", "z1", "r"});
}


QVector<QVector<QVector3D>> _CylinderSpanControl::lines() const
{
    QVector3D b(getValue("x"), getValue("y"), getValue("z1"));
    float s = getValue("r") / 4;

    return {{
        b + QVector3D(s, 0, -s),
        b + QVector3D(0, 0, 0),
        b + QVector3D(-s, 0, -s)
    },{
        b + QVector3D(0, s, -s),
        b + QVector3D(0, 0, 0),
        b + QVector3D(0, -s, -s)
    }};
}


void _CylinderSpanControl::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    dragValue("z1", d.z());
}

////////////////////////////////////////////////////////////////////////////////

CylinderControl::CylinderControl(Node* node, QObject* parent)
    : WireframeControl(node, parent),
      radius(new _CylinderRadiusControl(node, this)),
      span(new _CylinderSpanControl(node, this))

{
    watchDatums({"x", "y", "z0", "z1", "r"});
}


QVector<QVector<QVector3D>> CylinderControl::lines() const
{
    QVector3D a(getValue("x"), getValue("y"), getValue("z0"));
    QVector3D b(a.x(), a.y(), getValue("z1"));

    float s = getValue("r") / 4;

    return {{a + QVector3D(-s, 0, 0), a + QVector3D(s, 0, 0)},
            {a + QVector3D(0, -s, 0), a + QVector3D(0, s, 0)},
            {a, b}};
}

void CylinderControl::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    dragValue("x", d.x());
    dragValue("y", d.y());
    dragValue("z0", d.z());
    dragValue("z1", d.z());
}

