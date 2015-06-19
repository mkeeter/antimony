#ifndef EXPORT_HEIGHTMAP_H
#define EXPORT_HEIGHTMAP_H

#include <Python.h>

#include "export/export_worker.h"
#include "fab/types/shape.h"

////////////////////////////////////////////////////////////////////////////////

class ExportHeightmapWorker : public ExportWorker
{
public:
    explicit ExportHeightmapWorker(Shape s, Bounds b, QString f,
                                   float r, float mm)
        : ExportWorker(s, b, f, r), mm_per_unit(mm) {}

    void run() override;
protected:
    float mm_per_unit;
};

////////////////////////////////////////////////////////////////////////////////

class ExportHeightmapTask : public QObject
{
    Q_OBJECT
public:
    explicit ExportHeightmapTask(Shape s, Bounds b, float r,
                                 float mm, QString f, volatile int* halt)
        : shape(s), bounds(b), resolution(r),
          mm_per_unit(mm), filename(f), halt(halt)
        {}
public slots:
    void render();
signals:
    void finished();
protected:
    Shape shape;
    Bounds bounds;
    float resolution;
    float mm_per_unit;
    QString filename;

    volatile int* halt;
};

#endif
