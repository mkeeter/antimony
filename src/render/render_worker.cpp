#include <Python.h>

#include <QDebug>

#include "app.h"

#include "render/render_task.h"
#include "render/render_worker.h"
#include "render/render_image.h"

#include "ui/depth_image.h"

#include "datum/datum.h"
#include "datum/link.h"

#include "ui/canvas.h"

#include "fab/fab.h"

RenderWorker::RenderWorker(Datum* datum)
    : QObject(NULL), datum(datum), thread(NULL), current(NULL),
      next(NULL), depth_image(NULL), running(false),
      canvas(App::instance()->getCanvas())
{
    connect(datum, SIGNAL(changed()),
            this, SLOT(onDatumChanged()));
    connect(datum, SIGNAL(connectionChanged()),
            this, SLOT(onDatumChanged()));
    connect(datum, SIGNAL(destroyed()),
            this, SLOT(onDatumDeleted()));
    connect(canvas, SIGNAL(viewChanged()),
            this, SLOT(onDatumChanged()));
}

RenderWorker::~RenderWorker()
{
    if (depth_image)
    {
        depth_image->deleteLater();
    }
}

bool RenderWorker::accepts(Datum *d)
{
    return d->getType() == fab::ShapeType;
}

void RenderWorker::onDatumDeleted()
{
    if (!running)
    {
        deleteLater();
    }
}

bool RenderWorker::hasNoOutput()
{
    if (!datum)
    {
        return false;
    }

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
    if (datum->getValid() && datum->getValue() && hasNoOutput())
    {
        if (next)
        {
            next->deleteLater();
        }
        // Tell in-progress renders to abort.
        emit(abort());

        next = new RenderTask(datum->getValue(),
                                canvas->getTransformMatrix(),
                                canvas->getScale() / (1 << 4),
                                5);

        if (!running)
        {
            startNextRender();
        }
    }
}

void RenderWorker::onTaskFinished()
{
    if (!hasNoOutput())
    {
        clearImage();
    }

    if (current->hasFinishedRender() && hasNoOutput())
    {
        clearImage();
        depth_image = current->getDepthImage(canvas);
    }

    if (!next)
    {
        next = current->getNext();
    }

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

    // If the datum which we're rendering has been deleted, clean up
    // and call deleteLater on oneself.
    if (datum.isNull())
    {
        if (next)
        {
            next->deleteLater();
        }
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
    connect(this, SIGNAL(abort()),
            current, SIGNAL(halt()));

    connect(thread, SIGNAL(started()),
            current, SLOT(render()));

    connect(current, SIGNAL(finished()),
            this, SLOT(onTaskFinished()));

    connect(current, SIGNAL(destroyed()),
            thread, SLOT(quit()));

    connect(thread, SIGNAL(finished()),
            this, SLOT(onThreadFinished()));
    connect(thread, SIGNAL(finished()),
            thread, SLOT(deleteLater()));

    thread->start();
}
