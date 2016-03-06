#include "viewport/render/instance.h"
#include "viewport/render/task.h"

#include "viewport/view.h"
#include "graph/proxy/datum.h"
#include "graph/datum.h"

RenderInstance::RenderInstance(DatumProxy* parent, ViewportView* view)
    : QObject(), M(view->getMatrix()), image(this, view)
{
    connect(parent, &QObject::destroyed, this, &RenderInstance::makeOrphan);
    connect(view, &ViewportView::changed,
            this, &RenderInstance::viewChanged);
    datumChanged(parent->getDatum());
}

RenderInstance::~RenderInstance()
{
    Py_XDECREF(shape);
}

void RenderInstance::makeOrphan()
{
    orphan = true;
    if (!current)
    {
        deleteLater();
    }
}

void RenderInstance::datumChanged(Datum* d)
{
    bool should_render = d->outgoingLinks().size() == 0 &&
                         d->isValid() &&
                         d->currentValue() &&
                         d->isOutput();

    if (should_render)
    {
        shape = d->currentValue();
        Py_INCREF(shape);
    }
    else
    {
        Py_XDECREF(shape);
        shape = nullptr;
        image.clearTextures();
    }

    setPending();
}

void RenderInstance::viewChanged(QMatrix4x4 m)
{
    M = m;
    setPending();
}

void RenderInstance::setPending()
{
    if (shape)
    {
        // Mark that there is a task pending
        pending = true;

        if (current)
        {   // Tell in-progress renders to abort
            current->halt();
        }
        else
        {   // Otherwise, start the next render
            startNextRender();
        }
    }
}

void RenderInstance::onTaskFinished()
{
    if (orphan)
    {
        deleteLater();
    }
    else
    {
        image.update(current->pos, current->size,
                     current->depth, current->shaded,
                     current->color, current->flat);

        // Clear task pointer
        current.reset();

        // Start up next render
        if (pending && shape)
        {
            startNextRender();
        }
    }
}

void RenderInstance::startNextRender()
{
    // Lots of assertions!
    assert(pending);
    assert(shape);
    assert(current == nullptr);
    assert(orphan == false);

    current.reset(new RenderTask(this, shape, M));
    pending = false;
}
