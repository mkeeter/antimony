#ifndef RENDER_WORKER_H
#define RENDER_WORKER_H

#include <Python.h>
#include <QObject>
#include <QMatrix4x4>

#include "cpp/shape.h"

class RenderImage;

class RenderWorker : public QObject
{
    Q_OBJECT
public:
    explicit RenderWorker(PyObject* s, QMatrix4x4 m2d, QMatrix4x4 m3d);
    ~RenderWorker();

public slots:
    void render();
signals:
    void finished();
protected:
    void render2d(Shape s);
    void render3d(Shape s);

    PyObject* shape;
    QMatrix4x4 m2d, m3d;
    RenderImage* image;

    friend class RenderTask;
};

#endif // RENDER_WORKER_H
