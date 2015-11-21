#include <Python.h>

#include "graph/proxy/datum.h"
#include "graph/proxy/node.h"
#include "graph/proxy/util.h"

#include "graph/datum.h"
#include "canvas/inspector/row.h"

DatumProxy::DatumProxy(Datum* d, NodeProxy* parent)
    : QObject(parent), datum(d),
      row(new InspectorRow(QString::fromStdString(d->getName()),
                           parent->getInspector()))
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
    row->setText(QString::fromStdString(state.text));
}

void DatumProxy::setIndex(int i)
{
    row->setIndex(i);
}
