#include <Python.h>
#include "control/Deform/repel_control.h"

#include "ui/canvas.h"

RepelControl::RepelControl(Canvas* canvas, Node* node)
    : WireframeControl(canvas, node),
      radius(new RepelRadiusControl(canvas, node, this))
{
    watchDatums({"x","y","z","r"});
}

void RepelControl::drag(QVector3D c, QVector3D d)
{
    dragValue("x", d.x());
    dragValue("y", d.y());
    dragValue("z", d.z());
    Q_UNUSED(c);
}

QPointF RepelControl::inspectorPosition() const
{
    return canvas->worldToScene(
            QVector3D(getValue("x"), getValue("y"), getValue("z")));
}

QVector<QVector<QVector3D>> RepelControl::lines() const
{
    QVector3D center(getValue("x"), getValue("y"), getValue("z"));
    float r = getValue("r");

    QVector<QVector<QVector3D>> three;

    // Build arrows on one half of the XYZ plane, then mirror them.
    three << QVector<QVector3D>({QVector3D(r, 0, 0), QVector3D(r/2, 0, 0)})
        << QVector<QVector3D>({QVector3D(r*0.84, r/6., 0), QVector3D(r, 0, 0), QVector3D(r*0.84, -r/6, 0)})
        << QVector<QVector3D>({QVector3D(0, r, 0), QVector3D(0, r/2, 0)})
        << QVector<QVector3D>({QVector3D(r/6, r*0.84, 0), QVector3D(0, r, 0), QVector3D(-r/6, r*0.84, 0)})
        << QVector<QVector3D>({QVector3D(0, 0, r), QVector3D(0, 0, r/2)})
        << QVector<QVector3D>({QVector3D(r/6, 0, r*0.84), QVector3D(0, 0, r), QVector3D(-r/6, 0, r*0.84)});

    QVector<QVector<QVector3D>> out;
    for (auto line : three)
    {
        QVector<QVector3D> orig;
        QVector<QVector3D> mirr;
        for (auto pt : line)
        {
            orig << center + pt;
            mirr << center - pt;
        }
        out << orig << mirr;
    }
    return out;
}
