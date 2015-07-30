#include <Python.h>

#include <QDebug>

#include "render/render_task.h"
#include "render/render_proxy.h"
#include "render/render_worker.h"
#include "render/render_image.h"

#include "ui/viewport/depth_image.h"
#include "ui/viewport/viewport.h"

#include "graph/datum.h"
#include "graph/node.h"

#include "fab/fab.h"

RenderProxy::RenderProxy(RenderWorker* worker, Viewport* viewport)
    : QObject(NULL), worker(worker), thread(NULL), current(NULL),
      next(NULL), depth_image(NULL), running(false), starting_refinement(4),
      viewport(viewport)
{
    // If Viewport is destroyed, delete this worker if a task isn't running.
    // If a task is running, the deletion criterion will be checked on task
    // completion and the worker will be deleted then.
    connect(viewport, &QObject::destroyed,
            this, &RenderProxy::deleteIfNotRunning);
    connect(viewport, &Viewport::viewChanged,
            this, &RenderProxy::startRender);

    connect(worker, &QObject::destroyed,
            this, &RenderProxy::deleteIfNotRunning);
    connect(worker, &RenderWorker::changed,
            this, &RenderProxy::startRender);

    startRender();
}

RenderProxy::~RenderProxy()
{
    if (depth_image)
        depth_image->deleteLater();
}

void RenderProxy::deleteIfNotRunning()
{
    // If this worker isn't running, call deleteLater.
    // In the case that it is running, we check the deletion conditions
    // at the beginning of onThreadFinished (so we'll get deleted then).
    if (depth_image)
    {
        depth_image->deleteLater();
        depth_image = NULL;
    }

    if (!running)
        deleteLater();

    // Clear the datum pointer to request self-deletion.
    worker = NULL;
}

bool RenderProxy::hasNoOutput()
{
    if (!worker)
        return false;

    if (!worker->datum->outgoingLinks().empty())
    {
        clearImage();
        return false;
    }

    return true;
}

void RenderProxy::startRender()
{
    auto datum = worker->datum;
    if (hasNoOutput() && datum->isValid() &&
        datum->currentValue() && datum->isOutput())
    {
        if (next)
            next->deleteLater();

        // Tell in-progress renders to abort.
        emit(abort());

        next = new RenderTask(
                datum->currentValue(),
                viewport->getTransformMatrix(),
                viewport->getScale() / (1 << starting_refinement),
                starting_refinement + 1);

        if (!running)
            startNextRender();
    }
}

void RenderProxy::onTaskFinished()
{
    if (!hasNoOutput())
        clearImage();

    if (current->hasFinishedRender())
    {
        if (current->getRenderTime() < 25)
            starting_refinement = std::min(starting_refinement,
                                           current->getRefinement());
        else
            starting_refinement = std::min(current->getRefinement() + 1, 5);
    }

    if (current->hasFinishedRender() && hasNoOutput() && viewport)
    {
        clearImage();
        depth_image = current->getDepthImage(viewport);
    }
    else if (current->is_empty)
    {
        clearImage();
    }

    if (!next)
        next = current->getNext();

    current->deleteLater();
}

void RenderProxy::clearImage()
{
    if (depth_image)
    {
        depth_image->deleteLater();
        depth_image = NULL;
    }
}

void RenderProxy::onThreadFinished()
{
    running = false;

    // If the datum which we're rendering has been deleted or the
    // target viewport has been deleted, clean up and call deleteLater
    // on oneself.
    if (worker == NULL || worker->datum == NULL || viewport.isNull())
    {
        if (next)
            next->deleteLater();
        deleteLater();
    }

    // If there's a new worker to render, then start doing so.
    else if (next)
    {
        startNextRender();
    }
}

void RenderProxy::startNextRender()
{
    Q_ASSERT(!running);

    current = next;
    next = NULL;

    thread = new QThread();
    current->moveToThread(thread);

    running = true;

    // Halt rendering when the abort signal is emitted.
    connect(this, &RenderProxy::abort, current, &RenderTask::halt);

    connect(thread, &QThread::started, current, &RenderTask::render);

    connect(current, &RenderTask::finished,
            this, &RenderProxy::onTaskFinished);

    connect(current, &RenderProxy::destroyed, thread, &QThread::quit);

    connect(thread, &QThread::finished,
            this, &RenderProxy::onThreadFinished);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}
