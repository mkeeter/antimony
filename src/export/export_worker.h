#ifndef EXPORT_WORKER_H
#define EXPORT_WORKER_H

#include <Python.h>
#include <QObject>

#include "fab/types/shape.h"
#include "fab/types/bounds.h"

class ExportWorker : public QObject
{
    Q_OBJECT
public:
    explicit ExportWorker(Shape s, Bounds b, QString f, float r)
        : shape(s), bounds(b), filename(f), resolution(r) {}

    virtual void run()=0;

protected:
    Shape shape;
    Bounds bounds;
    QString filename;
    float resolution;
};

#endif
