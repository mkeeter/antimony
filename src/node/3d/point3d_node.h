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
    explicit Point3D(QString name, QObject* parent=NULL);
    explicit Point3D(QString name, QString x, QString y, QString z,
                     QObject* parent=NULL);
    explicit Point3D(float x, float y, float z, float scale,
                     QObject* parent=NULL);
};

#endif // POINT3D_H
