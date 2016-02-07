#ifndef EXPORT_VOXELS_H
#define EXPORT_VOXELS_H

#include <Python.h>

#include "export/export_worker.h"
#include "fab/types/shape.h"

////////////////////////////////////////////////////////////////////////////////

class ExportVoxelsWorker : public ExportWorker
{
public:
    explicit ExportVoxelsWorker(Shape s, Bounds b, QString f, float r)
        : ExportWorker(s, b, f, r) {}

    void run() override;

protected:
    bool detect_features;
};

////////////////////////////////////////////////////////////////////////////////

class ExportVoxelsTask : public QObject
{
    Q_OBJECT
public:
    explicit ExportVoxelsTask(Shape s, Bounds b, float r,
                            QString f, volatile int* halt)
        : shape(s), bounds(b), resolution(r),
          filename(f), halt(halt)
        {}
public slots:
    void render();
signals:
    void finished();
protected:
    Shape shape;
    Bounds bounds;
    float resolution;
    QString filename;

    volatile int* halt;
};

#endif
