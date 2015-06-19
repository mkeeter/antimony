#ifndef RENDER_WORKER_H
#define RENDER_WORKER_H

#include <Python.h>
#include <QObject>
#include <QMatrix4x4>

#include "fab/types/shape.h"
#include "fab/types/transform.h"

class RenderImage;
class DepthImageItem;
class Viewport;

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
    DepthImageItem* getDepthImage(Viewport* viewport);

    int getRenderTime() const { return time_taken; }
    int getRefinement() const { return refinement; }

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

    int time_taken;
    bool is_empty;

    friend class RenderWorker;
};

#endif // RENDER_WORKER_H
