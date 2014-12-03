#ifndef POINT3D_CONTROL_H
#define POINT3D_CONTROL_H

#include <Python.h>

#include <QObject>
#include <QVector3D>

#include "control/wireframe.h"

class Point3DControl : public WireframeControl
{
public:
    explicit Point3DControl(Node* node, QObject* parent=NULL);

    QVector<QPair<QVector3D, float>> points() const override;

    /*
     *  Drags the point in x, y, and z.
     */
    void drag(QVector3D center, QVector3D delta) override;

protected:
    QVector3D position() const;
};

#endif // POINT3D_CONTROL_H
