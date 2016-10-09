#pragma once

#include <Python.h>
#include <QtConcurrent>

#include <QObject>
#include <QImage>
#include <QMatrix4x4>
#include <QVector2D>
#include <QColor>

#include "fab/types/transform.h"
#include "fab/types/bounds.h"

class RenderInstance;
struct Shape;

class RenderTask : public QObject
{
Q_OBJECT
public:
    RenderTask(RenderInstance* parent, PyObject* s, QMatrix4x4 M,
               QVector2D clip, int refinement=1);
    ~RenderTask();

    /*
     *  Request that the rendering task halts early
     */
    void halt();

    /*
     *  Returns a render task at the next level of refinement
     *  (or null if this task is at refinement = 0)
     */
    RenderTask* getNext(RenderInstance* parent) const;

protected:
    /*
     *  Async rendering task
     */
    void async();

    /*
     *  Prepare the shape and call render() for 2D or 3D objects
     */
    void render2d(const Shape& s);
    void render3d(const Shape& s);

    /*
     *  Renders a shape, storing results in depth and shaded
     *  Returns screen-clipped bounds
     */
    Bounds render(Shape* shape, Bounds b, float scale);

    /*
     *  Converts the given matrix into a Transform
     */
    static Transform getTransform(QMatrix4x4 m);

    PyObject* shape;
    QMatrix4x4 M;
    QVector2D clip;

    QFuture<void> future;
    QFutureWatcher<void> watcher;

    /*  Position of image center (in screen coordinates)  */
    QVector3D pos;

    /*  Size of rendered image  */
    QVector3D size;

    /*  Output data (pulled into DepthImages for rendering)  */
    QImage depth;
    QImage shaded;
    QColor color;
    bool flat;

    /*  Flag used to abort rendering asynchronously  */
    int halt_flag=0;

    /*  Render time (used to decide how to adjust starting refinement  */
    int render_time=-1;

    /*  Scale factor used to reduce resolution  *
     *  1 is pixel-perfect resolution           */
    int refinement;

    /*  Give RenderInstance access to internal members  */
    friend class RenderInstance;
};
