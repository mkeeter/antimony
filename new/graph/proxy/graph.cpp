#include <Python.h>

#include "graph/proxy/graph.h"
#include "graph/proxy/node.h"
#include "graph/proxy/subdatum.h"
#include "graph/proxy/datum.h"
#include "graph/proxy/util.h"
#include "graph/hooks/hooks.h"

#include "canvas/scene.h"
#include "canvas/inspector/buttons.h"
#include "window/canvas.h"

#include "viewport/scene.h"
#include "window/viewport.h"

#include "graph/graph.h"
#include "graph/graph_node.h"

GraphProxy::GraphProxy(Graph* g, NodeProxy* parent)
    : QObject(parent), graph(g), canvas_scene(new CanvasScene(g, this)),
      viewport_scene(new ViewportScene(g, this)), hooks(new AppHooks(this))
{
    g->installWatcher(this);
    g->installExternalHooks(hooks);

    if (parent != NULL)
    {
        new InspectorGraphButton(this, parent->getInspector()->getTitleRow());
        new InspectorViewButton(this, parent->getInspector()->getTitleRow());
        new InspectorQuadButton(this, parent->getInspector()->getTitleRow());
    }
}

void GraphProxy::trigger(const GraphState& state)
{
    updateHash(state.nodes,  &nodes,  this);
    updateHash(state.datums, &datums, this);
}

////////////////////////////////////////////////////////////////////////////////

template <class W, class S>
W* GraphProxy::newWindow(S* scene)
{
    auto win = new W(scene);
    connect(this, &GraphProxy::destroyed, win, &QObject::deleteLater);

    return win;
}

CanvasWindow* GraphProxy::newCanvasWindow()
{
    return newWindow<CanvasWindow>(canvas_scene);
}

ViewportWindow* GraphProxy::newViewportWindow()
{
    auto view = viewport_scene->getView();
    auto win = newWindow<ViewportWindow>(view);

    for (auto n : nodes)
    {
        n->makeInstancesFor(view);
    }

    return win;
}

void GraphProxy::makeInstancesFor(Control* c)
{
    viewport_scene->makeInstancesFor(c);
}

////////////////////////////////////////////////////////////////////////////////

NodeProxy* GraphProxy::getNodeProxy(Node* n)
{
    if (!nodes.contains(n))
        nodes[n] = new NodeProxy(n, this);
    return nodes[n];
}

BaseDatumProxy* GraphProxy::getDatumProxy(Datum* d)
{
    if (d->parentNode()->parentGraph() == graph)
    {
        return getNodeProxy(d->parentNode())->getDatumProxy(d);
    }
    else if (auto gn = dynamic_cast<GraphNode*>(d->parentNode()))
    {
        Q_ASSERT(gn->getGraph() == graph);
        if (!datums.contains(d))
            datums[d] = new SubdatumProxy(d, this);
        return datums[d];
    }

    Q_ASSERT(false);
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////

CanvasInfo GraphProxy::canvasInfo() const
{
    CanvasInfo out;
    for (auto n : nodes)
        out.unite(n->canvasInfo());

    for (auto d : datums)
        out.unite(d->canvasInfo());

    return out;
}

void GraphProxy::setPositions(const CanvasInfo& info, bool select)
{
    for (auto n : nodes)
        n->setPositions(info, select);
    for (auto d : datums)
        d->setPositions(info, select);
}
