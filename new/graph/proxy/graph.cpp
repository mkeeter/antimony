#include <Python.h>

#include "graph/proxy/graph.h"
#include "graph/proxy/node.h"
#include "graph/proxy/superdatum.h"
#include "graph/proxy/util.h"
#include "graph/hooks/hooks.h"

#include "canvas/scene.h"
#include "window/canvas.h"

#include "graph/graph.h"

GraphProxy::GraphProxy(Graph* g, QObject* parent)
    : QObject(parent), canvas_scene(new CanvasScene(g, this)),
      hooks(new AppHooks(this))
{
    g->installWatcher(this);
    g->installExternalHooks(hooks);
}

GraphProxy::~GraphProxy()
{
    for (auto w : windows)
        w->close();
    delete hooks;
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
