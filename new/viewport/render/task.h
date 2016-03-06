#pragma once

#include <Python.h>
#include <QtConcurrent>

#include <QObject>
#include <QMatrix4x4>

#include "fab/types/transform.h"
#include "fab/types/bounds.h"

class RenderInstance;
struct Shape;

class RenderTask : public QObject
{
Q_OBJECT
public:
    RenderTask(RenderInstance* parent, PyObject* s, QMatrix4x4 M);
    ~RenderTask();

    /*
     *  Request that the rendering task halts early
     */
    void halt();

protected:
    /*
     *  Async rendering task
     */
    void async();

    /*
     *  Prepare the shape and call render() for 2D or 3D objects
     */
    void render2d(const Shape& s, const QMatrix4x4& matrix);
    void render3d(const Shape& s, const QMatrix4x4& matrix);

    /*
     *  Renders a shape, returning a pair of images for depth and normals
     */
    QPair<QImage, QImage> render(
            Shape* shape, Bounds b, float scale);

    /*
     *  Converts the given matrix into a Transform
     */
    static Transform getTransform(QMatrix4x4 m);

    PyObject* shape;
    QMatrix4x4 M;

    QFuture<void> future;
    QFutureWatcher<void> watcher;

    /*  Flag used to abort rendering asynchronously  */
    int halt_flag=0;
};
