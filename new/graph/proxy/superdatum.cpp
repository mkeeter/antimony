#include <Python.h>

#include "graph/proxy/superdatum.h"
#include "graph/proxy/graph.h"

#include "graph/datum.h"

SuperDatumProxy::SuperDatumProxy(Datum* d, GraphProxy* parent)
    : QObject(parent)
{
    d->installWatcher(this);
}

void SuperDatumProxy::trigger(const DatumState& state)
{
    (void)state;
}
