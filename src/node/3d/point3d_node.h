#ifndef POINT3D_H
#define POINT3D_H

#include <Python.h>
#include <QObject>

#include "node/node.h"

class Canvas;

class Point3D : public Node
{
    Q_OBJECT
public:
    explicit Point3D(QString name, QString x, QString y, QString z,
                     QObject* parent=NULL);
    explicit Point3D(float x, float y, float z, float scale,
                     QObject* parent=NULL);

    virtual Control* makeControl(Canvas *canvas) override;

    static QString menuName()     { return "Point (3D)"; }
    static QString menuCategory() { return "3D"; }

};

#endif // POINT3D_H
