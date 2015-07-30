#ifndef RENDER_TASK_H
#define RENDER_TASK_H

#include <QObject>
#include <QPointer>
#include <QThread>

#include "graph/watchers.h"

class RenderWorker;
class RenderTask;
class DepthImageItem;
class Viewport;

class RenderProxy : public QObject
{
    Q_OBJECT
public:
    explicit RenderProxy(RenderWorker* worker, Viewport* viewport);
    ~RenderProxy();
public slots:
    void startRender();
    void deleteIfNotRunning();
    void onTaskFinished();
    void onThreadFinished();

signals:
    void abort();

protected:
    /** Checks to see if the datum has output.
     *  If so, returns false and deletes image.
     */
    bool hasNoOutput();

    /** Starts rendering the task in next.
     *  Moves next to current when starting.
     */
    void startNextRender();

    /** Calls deleteLater on image and sets it to NULL.
     */
    void clearImage();

    RenderWorker* worker;

    QThread* thread;
    RenderTask* current;
    RenderTask* next;
    QPointer<DepthImageItem> depth_image;

    bool running;
    int starting_refinement;

    QPointer<Viewport> viewport;
};

#endif // RENDER_TASK_H
