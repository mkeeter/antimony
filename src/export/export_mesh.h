#ifndef EXPORT_MESH_H
#define EXPORT_MESH_H

#include <Python.h>

#include "export/export_worker.h"
#include "fab/types/shape.h"

////////////////////////////////////////////////////////////////////////////////

class ExportMeshWorker : public ExportWorker
{
public:
    explicit ExportMeshWorker(Shape s, Bounds b, QString f, float r, bool d)
        : ExportWorker(s, b, f, r), detect_features(d) {}

    void run() override;

protected:
    bool detect_features;
};

////////////////////////////////////////////////////////////////////////////////

class ExportMeshTask : public QObject
{
    Q_OBJECT
public:
    explicit ExportMeshTask(Shape s, Bounds b, float r, bool d,
                            QString f, volatile int* halt)
        : shape(s), bounds(b), resolution(r),
          detect_features(d), filename(f), halt(halt)
        {}
public slots:
    void render();
signals:
    void finished();
protected:
    Shape shape;
    Bounds bounds;
    float resolution;
    bool detect_features;
    QString filename;

    volatile int* halt;
};

#endif
