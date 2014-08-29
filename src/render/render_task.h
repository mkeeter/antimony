#ifndef RENDER_WORKER_H
#define RENDER_WORKER_H

#include <Python.h>
#include <QObject>
#include <QMatrix4x4>

#include "cpp/shape.h"
#include "cpp/transform.h"

class RenderImage;
class DepthImageItem;
class Canvas;

class RenderTask : public QObject
{
    Q_OBJECT
public:
    explicit RenderTask(PyObject* s, QMatrix4x4 matrix,
                          float scale, int refinement);
    ~RenderTask();

    RenderTask* getNext() const;

    /** Returns True if image is not NULL and was not halted mid-render.
     */
    bool hasFinishedRender() const;

    /** Constructs a depth image in the given canvas and returns it.
     */
    DepthImageItem* getDepthImage(Canvas* canvas);

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

    friend class RenderWorker;
};

#endif // RENDER_WORKER_H
