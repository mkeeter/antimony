#include <Python.h>

#include "graph/proxy/graph.h"
#include "graph/proxy/datum.h"
#include "graph/proxy/node.h"
#include "graph/proxy/util.h"

#include "graph/datum.h"
#include "canvas/datum_row.h"
#include "canvas/connection/connection.h"

#include "viewport/render/instance.h"
#include "viewport/view.h"
#include "viewport/scene.h"

#include "fab/fab.h"

DatumProxy::DatumProxy(Datum* d, NodeProxy* parent)
    : BaseDatumProxy(d, parent), row(new DatumRow(d, parent->getInspector())),
      should_render(d->getType() == fab::ShapeType)
{
    d->installWatcher(this);

    /*
     *  Install the render instance for every existing viewport
     */
    for (auto v : static_cast<GraphProxy*>(parent->parent())
                    ->viewportScene()->getViews())
    {
        addViewport(v);
    }
}

void DatumProxy::trigger(const DatumState& state)
{
    row->update(state);

    if (state.sigil == Datum::SIGIL_NONE ||
        state.sigil == Datum::SIGIL_CONNECTION)
    {
        updateHash(state.links, &connections, this, true);
    }

    for (auto r : render)
    {
        r->datumChanged(getDatum());
    }
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

////////////////////////////////////////////////////////////////////////////////

void DatumProxy::addViewport(ViewportView* view)
{
    if (should_render)
    {
        render[view] = new RenderInstance(this, view);
        connect(view, &QObject::destroyed, [=]{
                this->render[view]->makeOrphan();
                this->render.remove(view); });
    }
}
