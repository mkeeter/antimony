#include "viewport/render/instance.h"
#include "viewport/render/task.h"

#include "viewport/view.h"
#include "graph/proxy/subdatum.h"
#include "graph/datum.h"

RenderInstance::RenderInstance(
        BaseDatumProxy* parent, ViewportView* view, bool sub)
    : QObject(), sub(sub), M(view->getMatrix()),
      clip(view->geometry().width(), view->geometry().height()),
      image(this, view)
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
                        (sub ? !d->isFromSubgraph() : d->isOutput());

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

void RenderInstance::viewChanged(QMatrix4x4 m, QRect clip_)
{
    M = m;
    clip = {float(clip_.width()), float(clip_.height())};
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
        bool restarted = false;

        if (!current->halt_flag)
        {
            image.update(current->pos, current->size,
                         current->depth, current->shaded,
                         current->color, current->flat);

            // Dynamically adjust default refinement level based on render time
            if (current->render_time < 25)
            {
                starting_refinement = std::max(1, starting_refinement - 1);
            }
            else
            {
                starting_refinement = std::min(starting_refinement + 1, 5);
            }

            // If we don't have a pending render task, then begin
            // a refinement render task.
            if (!pending)
            {
                current.reset(current->getNext(this));
                restarted = true;
            }
        }
        else
        {   // If we didn't have enough time to render before the abort flag
            // went up, then increase the starting refinement here
            starting_refinement = std::min(starting_refinement + 1, 5);
        }

        if (!restarted)
        {
            // Clear task pointer
            current.reset();

            // Start up next render
            if (pending && shape)
            {
                startNextRender();
            }
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

    Q_ASSERT(starting_refinement >= 1);
    current.reset(new RenderTask(this, shape, M, clip, starting_refinement));

    pending = false;
}
