#include <Python.h>
#include "control/Deform/attract_control.h"

#include "ui/canvas.h"

AttractControl::AttractControl(Canvas* canvas, Node* node)
    : MultiLineControl(canvas, node),
      radius(new AttractRadiusControl(canvas, node, this))
{
    watchDatums({"x","y","z","r"});
}

void AttractControl::drag(QVector3D c, QVector3D d)
{
    dragValue("x", d.x());
    dragValue("y", d.y());
    dragValue("z", d.z());
    Q_UNUSED(c);
}

QPointF AttractControl::inspectorPosition() const
{
    return canvas->worldToScene(
            QVector3D(getValue("x"), getValue("y"), getValue("z")));
}

QVector<QVector<QVector3D>> AttractControl::lines() const
{
    QVector3D center(getValue("x"), getValue("y"), getValue("z"));
    float r = getValue("r");

    QVector<QVector<QVector3D>> three;

    // Build arrows on one half of the XYZ plane, then mirror them.
    three << QVector<QVector3D>({QVector3D(r, 0, 0), QVector3D(r/2, 0, 0)})
        << QVector<QVector3D>({QVector3D(r*0.66, r/6., 0), QVector3D(r/2, 0, 0), QVector3D(r*0.66, -r/6, 0)})
        << QVector<QVector3D>({QVector3D(0, r, 0), QVector3D(0, r/2, 0)})
        << QVector<QVector3D>({QVector3D(r/6, r*0.66, 0), QVector3D(0, r/2, 0), QVector3D(-r/6, r*0.66, 0)})
        << QVector<QVector3D>({QVector3D(0, 0, r), QVector3D(0, 0, r/2)})
        << QVector<QVector3D>({QVector3D(r/6, 0, r*0.66), QVector3D(0, 0, r/2), QVector3D(-r/6, 0, r*0.66)});

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
