#include <Python.h>

#include "graph/proxy/superdatum.h"
#include "graph/proxy/graph.h"
#include "graph/proxy/util.h"

#include "graph/datum.h"
#include "canvas/datum_frame.h"
#include "canvas/scene.h"

SuperDatumProxy::SuperDatumProxy(Datum* d, GraphProxy* parent)
    : QObject(parent), frame(new DatumFrame(d, parent->canvasScene()))
{
    d->installWatcher(this);
    NULL_ON_DESTROYED(frame);
}

SuperDatumProxy::~SuperDatumProxy()
{
    if (frame)
        delete frame;
}

void SuperDatumProxy::trigger(const DatumState& state)
{
    frame->update(state);
}
