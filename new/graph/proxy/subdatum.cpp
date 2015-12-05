#include <Python.h>

#include "graph/proxy/subdatum.h"
#include "graph/proxy/graph.h"
#include "graph/proxy/util.h"

#include "graph/datum.h"
#include "canvas/subdatum/subdatum_frame.h"
#include "canvas/scene.h"

SubdatumProxy::SubdatumProxy(Datum* d, GraphProxy* parent)
    : QObject(parent), frame(new SubdatumFrame(d, parent->canvasScene()))
{
    d->installWatcher(this);
    NULL_ON_DESTROYED(frame);
}

SubdatumProxy::~SubdatumProxy()
{
    if (frame)
        delete frame;
}

void SubdatumProxy::trigger(const DatumState& state)
{
    frame->update(state);
}
