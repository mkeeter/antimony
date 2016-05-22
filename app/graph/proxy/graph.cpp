#include <Python.h>

#include "graph/proxy/graph.h"
#include "graph/proxy/node.h"
#include "graph/proxy/subdatum.h"
#include "graph/proxy/datum.h"
#include "graph/proxy/util.h"
#include "graph/hooks/hooks.h"

#include "canvas/scene.h"
#include "canvas/canvas_view.h"
#include "canvas/inspector/buttons.h"
#include "window/canvas.h"

#include "viewport/scene.h"
#include "viewport/view.h"

#include "window/viewport.h"
#include "window/quad.h"

#include "graph/graph.h"
#include "graph/graph_node.h"

////////////////////////////////////////////////////////////////////////////////

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
    connect(this, &GraphProxy::subnameChanged, win, &BaseWindow::setSub);

    // If we're in a subgraph, update the window title
    if (auto p = dynamic_cast<NodeProxy*>(parent()))
    {
        p->onSubnameChanged();
    }

    return win;
}

CanvasWindow* GraphProxy::newCanvasWindow()
{
    auto view = canvas_scene->getView();
    connect(this, &GraphProxy::zoomTo, view, &CanvasView::zoomTo);
    return newWindow<CanvasWindow>(view);
}

ViewportWindow* GraphProxy::newViewportWindow()
{
    auto view = viewport_scene->getView();
    connect(this, &GraphProxy::zoomTo, view, &ViewportView::zoomTo);

    auto win = newWindow<ViewportWindow>(view);

    for (auto n : nodes)
    {
        n->makeInstancesFor(view);
    }

    return win;
}

QuadWindow* GraphProxy::newQuadWindow()
{
    auto front = viewport_scene->getView();
    auto top = viewport_scene->getView();
    auto side = viewport_scene->getView();
    auto ortho = viewport_scene->getView();

    for (auto v : {front, top, side, ortho})
    {
        connect(this, &GraphProxy::zoomTo, v, &ViewportView::zoomTo);
    }

    auto win = new QuadWindow(front, top, side, ortho);
    connect(this, &GraphProxy::destroyed, win, &QObject::deleteLater);
    connect(this, &GraphProxy::subnameChanged,
            win, &BaseWindow::setSub);

    // If we're in a subgraph, update the window title
    if (auto p = dynamic_cast<NodeProxy*>(parent()))
    {
        p->onSubnameChanged();
    }

    for (auto n : nodes)
    {
        for (auto v : {front, top, side, ortho})
        {
            n->makeInstancesFor(v);
        }
    }

    return win;
}


void GraphProxy::makeInstancesFor(Control* c)
{
    viewport_scene->installControl(c);
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
