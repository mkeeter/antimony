#include <Python.h>

#include "graph/proxy/graph.h"
#include "graph/proxy/datum.h"
#include "graph/proxy/node.h"
#include "graph/proxy/util.h"

#include "graph/datum.h"
#include "canvas/datum_row.h"
#include "canvas/connection/connection.h"

#include "viewport/scene.h"

DatumProxy::DatumProxy(Datum* d, NodeProxy* parent)
    : BaseDatumProxy(d, parent, static_cast<GraphProxy*>(
                parent->parent())->viewportScene(), false),
      row(new DatumRow(d, parent->getInspector()))
{
    d->installWatcher(this);
    connect(this, &QObject::destroyed, row, &QObject::deleteLater);
    connect(this, &QObject::destroyed,
            parent->getInspector(), &InspectorFrame::redoLayout,
            Qt::QueuedConnection);
}

void DatumProxy::trigger(const DatumState& state)
{
    row->update(state);

    if (state.sigil == Datum::SIGIL_NONE ||
        state.sigil == Datum::SIGIL_CONNECTION)
    {
        updateHash(state.links, &connections, this, true);
    }

    emit(datumChanged(getDatum()));
}

void DatumProxy::setIndex(int i)
{
    row->setIndex(i);
}

////////////////////////////////////////////////////////////////////////////////

InputPort* DatumProxy::inputPort() const
{
    return row->inputPort();
}

OutputPort* DatumProxy::outputPort() const
{
    return row->outputPort();
}

GraphProxy* DatumProxy::graphProxy() const
{
    return static_cast<GraphProxy*>(parent()->parent());
}
