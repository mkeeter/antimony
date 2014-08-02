#include <Python.h>

#include <QVector3D>

#include "control/3d/extrude_control.h"
#include "ui/canvas.h"
#include "ui/colors.h"

_ExtrudeSpanControl::_ExtrudeSpanControl(Canvas* canvas, Node* node, QGraphicsItem* parent)
    : WireframeControl(canvas, node, parent)
{
    watchDatums({"_x", "_y", "z0", "z1", "_scale"});
}


QVector<QVector<QVector3D>> _ExtrudeSpanControl::lines() const
{
    QVector3D b(getValue("_x"), getValue("_y"), getValue("z1"));
    float s = getValue("_scale");

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


void _ExtrudeSpanControl::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    dragValue("z1", d.z());
}

QColor _ExtrudeSpanControl::defaultPenColor() const
{
    return Colors::orange;
}

////////////////////////////////////////////////////////////////////////////////

ExtrudeControl::ExtrudeControl(Canvas* canvas, Node* node)
    : WireframeControl(canvas, node),
      span(new _ExtrudeSpanControl(canvas, node, this))

{
    watchDatums({"_x", "_y", "z0", "z1", "_scale"});
}


QVector<QVector<QVector3D>> ExtrudeControl::lines() const
{
    QVector3D a(getValue("_x"), getValue("_y"), getValue("z0"));
    QVector3D b(a.x(), a.y(), getValue("z1"));
    float s = 2*getValue("_scale");

    return {{a + QVector3D(-s, 0, 0), a + QVector3D(s, 0, 0)},
            {a + QVector3D(0, -s, 0), a + QVector3D(0, s, 0)},
            {a, b}};
}

void ExtrudeControl::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    dragValue("_x", d.x());
    dragValue("_y", d.y());
    dragValue("z0", d.z());
    dragValue("z1", d.z());
}

QPointF ExtrudeControl::inspectorPosition() const
{
    return canvas->worldToScene(QVector3D(
                getValue("_x"), getValue("_y"), getValue("z0")));
}


QColor ExtrudeControl::defaultPenColor() const
{
    return Colors::orange;
}

