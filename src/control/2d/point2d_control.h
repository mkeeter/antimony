#ifndef POINT2D_CONTROL_H
#define POINT2D_CONTROL_H

#include <Python.h>

#include <QObject>
#include <QVector3D>

#include "control/wireframe.h"

class Point2DControl : public WireframeControl
{
public:
    explicit Point2DControl(Node* node, QObject* parent=0,
                            QString suffix="");

    QVector<QPair<QVector3D, float>> points() const override;

    /*
     *  Drags the point in x and y.
     */
    void drag(QVector3D center, QVector3D delta) override;

protected:
    QVector3D position() const;
    const QString suffix;
};

#endif // POINT2D_CONTROL_H
