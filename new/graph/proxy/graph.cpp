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

#include "graph/graph.h"
#include "graph/graph_node.h"

GraphProxy::GraphProxy(Graph* g, NodeProxy* parent)
    : QObject(parent), graph(g), canvas_scene(new CanvasScene(g, this)),
      hooks(new AppHooks(this))
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

GraphProxy::~GraphProxy()
{
    for (auto w : windows)
    {
        disconnect(w, &QMainWindow::destroyed, 0, 0);
        w->close();
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

    // Automatically prune the window list when the window is closed
    connect(win, &QMainWindow::destroyed,
            [=]{ this->windows.removeAll(win); });
    windows.append(win);

    return win;
}

CanvasWindow* GraphProxy::newCanvasWindow()
{
    return newWindow<CanvasWindow>(canvas_scene);
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

void GraphProxy::setPositions(const CanvasInfo& info)
{
    for (auto n : nodes)
        n->setPositions(info);
    for (auto d : datums)
        d->setPositions(info);
}
