#include <Python.h>

#include "graph/proxy/datum.h"
#include "graph/proxy/node.h"

#include "graph/datum.h"

DatumProxy::DatumProxy(Datum* d, NodeProxy* parent)
    : QObject(parent)
{
    d->installWatcher(this);
}

void DatumProxy::trigger(const DatumState& state)
{
    (void)state;
}
