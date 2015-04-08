#include <Python.h>

#include <QDebug>

#include "render/render_task.h"
#include "render/render_worker.h"
#include "render/render_image.h"

#include "ui/viewport/depth_image.h"

#include "graph/datum/datum.h"
#include "graph/datum/link.h"

#include "ui/viewport/viewport.h"

#include "fab/fab.h"

RenderWorker::RenderWorker(Datum* datum, Viewport* viewport)
    : QObject(NULL), datum(datum), thread(NULL), current(NULL),
      next(NULL), depth_image(NULL), running(false), starting_refinement(4),
      viewport(viewport)
{
    connect(datum, &Datum::changed,
            this, &RenderWorker::onDatumChanged);
    connect(datum, &Datum::connectionChanged,
            this, &RenderWorker::onDatumChanged);

    // If the Datum or Viewport is destroyed, delete this worker if a task
    // isn't running.  If a task is running, the deletion criterion will be
    // checked on task completion and the worker will be deleted then.
    connect(datum, &Datum::destroyed,
            this, &RenderWorker::deleteIfNotRunning);
    connect(viewport, &Viewport::destroyed,
            this, &RenderWorker::deleteIfNotRunning);
    connect(viewport, &Viewport::viewChanged,
            this, &RenderWorker::onDatumChanged);

    onDatumChanged();
}

RenderWorker::~RenderWorker()
{
    if (depth_image)
        depth_image->deleteLater();
}

bool RenderWorker::accepts(Datum *d)
{
    return d->getType() == fab::ShapeType;
}

void RenderWorker::deleteIfNotRunning()
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
}

bool RenderWorker::hasNoOutput()
{
    if (!datum)
        return false;

    if (!datum->hasOutput())
    {
        clearImage();
        return false;
    }

    if (datum->hasConnectedLink())
    {
        clearImage();
        return false;
    }
    return true;
}

void RenderWorker::onDatumChanged()
{
    if (datum && datum->getValid() && datum->getValue() && hasNoOutput())
    {
        if (next)
            next->deleteLater();

        // Tell in-progress renders to abort.
        emit(abort());

        next = new RenderTask(
                datum->getValue(),
                viewport->getTransformMatrix(),
                viewport->getScale() / (1 << starting_refinement),
                starting_refinement + 1);

        if (!running)
            startNextRender();
    }
}

void RenderWorker::onTaskFinished()
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

void RenderWorker::clearImage()
{
    if (depth_image)
    {
        depth_image->deleteLater();
        depth_image = NULL;
    }
}

void RenderWorker::onThreadFinished()
{
    running = false;

    // If the datum which we're rendering has been deleted or the
    // target viewport has been deleted, clean up and call deleteLater
    // on oneself.
    if (datum.isNull() || viewport.isNull())
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

void RenderWorker::startNextRender()
{
    Q_ASSERT(!running);

    current = next;
    next = NULL;

    thread = new QThread();
    current->moveToThread(thread);

    running = true;

    // Halt rendering when the abort signal is emitted.
    connect(this, &RenderWorker::abort, current, &RenderTask::halt);

    connect(thread, &QThread::started, current, &RenderTask::render);

    connect(current, &RenderTask::finished,
            this, &RenderWorker::onTaskFinished);

    connect(current, &RenderWorker::destroyed, thread, &QThread::quit);

    connect(thread, &QThread::finished,
            this, &RenderWorker::onThreadFinished);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}
