#include <Python.h>

#include "graph/proxy/datum.h"
#include "graph/proxy/node.h"
#include "graph/proxy/util.h"

#include "graph/datum.h"
#include "canvas/datum_row.h"

DatumProxy::DatumProxy(Datum* d, NodeProxy* parent)
    : QObject(parent), datum(d),
      row(new DatumRow(d, parent->getInspector()))
{
    d->installWatcher(this);

    NULL_ON_DESTROYED(row);
}

DatumProxy::~DatumProxy()
{
    if (row)
        delete row;
}

void DatumProxy::trigger(const DatumState& state)
{
    row->update(state);
}

void DatumProxy::setIndex(int i)
{
    row->setIndex(i);
}
