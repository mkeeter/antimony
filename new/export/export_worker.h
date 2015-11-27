#pragma once

#include <Python.h>
#include <QObject>

#include "fab/types/shape.h"
#include "fab/types/bounds.h"

/*
 *  Abstract base class used to export files from a Shape
 */
class ExportWorker : public QObject
{
    Q_OBJECT
public:
    explicit ExportWorker(Shape s, Bounds b, QString f, float r)
        : shape(s), bounds(b), filename(f), resolution(r) {}

    /*
     *  Overloaded by children to export a specific file
     */
    virtual void run()=0;

protected:
    Shape shape;
    Bounds bounds;
    QString filename;
    float resolution;
};
