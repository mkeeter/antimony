#ifndef RENDER_WORKER_H
#define RENDER_WORKER_H

#include <Python.h>
#include <QObject>
#include <QMatrix4x4>

#include "cpp/shape.h"
#include "cpp/transform.h"

class RenderImage;

class RenderWorker : public QObject
{
    Q_OBJECT
public:
    explicit RenderWorker(PyObject* s, QMatrix4x4 matrix,
                          float scale, int refinement);
    ~RenderWorker();

    RenderWorker* getNext() const;
public slots:
    void render();
signals:
    void finished();
    void halt();
protected:
    void render2d(Shape s);
    void render3d(Shape s);

    /** Returns a Transform object that applies the given matrix.
     */
    static Transform getTransform(QMatrix4x4 m);

    PyObject* shape;
    QMatrix4x4 matrix;
    float scale;
    int refinement;

    RenderImage* image;

    friend class RenderTask;
};

#endif // RENDER_WORKER_H
