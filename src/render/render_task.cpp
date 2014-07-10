#include <Python.h>

#include <QDebug>

#include "app.h"

#include "render/render_task.h"
#include "render/render_worker.h"
#include "datum/datum.h"

#include "ui/canvas.h"

#include "cpp/fab.h"

RenderTask::RenderTask(Datum* datum)
    : QObject(NULL), datum(datum), thread(NULL), current(NULL),
      next(NULL), running(false),
      canvas(dynamic_cast<App*>(QApplication::instance())->getCanvas())
{
    connect(datum, SIGNAL(changed()),
            this, SLOT(onDatumChanged()));
    connect(datum, SIGNAL(destroyed()),
            this, SLOT(onDatumDeleted()));
    connect(canvas, SIGNAL(viewChanged()),
            this, SLOT(onDatumChanged()));
}

bool RenderTask::accepts(Datum *d)
{
    return dynamic_cast<App*>(QApplication::instance()) &&
           d->getType() == fab::ShapeType;
}

void RenderTask::onDatumDeleted()
{
    if (!running)
    {
        deleteLater();
    }
}

void RenderTask::onDatumChanged()
{
    if (datum->getValid() && datum->getValue())
    {
        if (next)
        {
            next->deleteLater();
        }
        next = new RenderWorker(datum->getValue(), canvas->getMatrix());

        if (!running)
        {
            startNextRender();
        }
    }
}

void RenderTask::onThreadFinished()
{
    // TODO: Get image from thread.
    running = false;

    thread->deleteLater();
    current->deleteLater();

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

void RenderTask::startNextRender()
{
    Q_ASSERT(!running);

    current = next;
    next = NULL;

    thread = new QThread();
    current->moveToThread(thread);

    running = true;

    connect(thread, SIGNAL(started()),
            current, SLOT(render()));
    connect(thread, SIGNAL(finished()),
            this, SLOT(onThreadFinished()));
    connect(current, SIGNAL(finished()),
            thread, SLOT(quit()));

    thread->start();
}
