#include <Python.h>

#include "render/render_worker.h"
#include "render/render_proxy.h"

#include "graph/datum.h"
#include "fab/fab.h"

RenderWorker::RenderWorker(Datum* datum)
    : QObject(NULL), datum(datum)
{
    datum->installWatcher(this);
}

void RenderWorker::trigger(const DatumState& state)
{
    Q_UNUSED(state);
    emit(changed());
}

bool RenderWorker::accepts(Datum *d)
{
    return d->getType() == fab::ShapeType && d->isOutput();
}
