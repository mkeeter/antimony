#include <algorithm>

#include "graph/graph.h"
#include "graph/node.h"
#include "graph/proxy.h"
#include "graph/hooks/hooks.h"
#include "graph/watchers.h"

Graph::Graph(std::string n, Graph* parent)
    : name(n), uid(0), parent(parent)
{
    // Nothing to do here
}

PyObject* Graph::proxyDict(Node* locals, Downstream* caller)
{
    return Proxy::makeProxyFor(this, locals, caller);
}

void Graph::triggerWatchers() const
{
    if (!watchers.empty())
    {
        auto state = getState();
        for (auto w : watchers)
            w->trigger(state);
    }
}

uint32_t Graph::install(Node* n)
{
    const uint32_t uid = Root::install(n, &nodes);
    triggerWatchers();
    return uid;
}

GraphState Graph::getState() const
{
    std::unordered_set<Node*> out;
    for (const auto& ptr : nodes)
        out.insert(ptr.get());
    return (GraphState){ out };
}

void Graph::uninstall(Node* n)
{
    Root::uninstall(n, &nodes);
    triggerWatchers();
}

PyObject* Graph::pyGetAttr(std::string name, Downstream* caller) const
{
    auto m = get(name, nodes);
    return m ? Proxy::makeProxyFor(m, NULL, caller) : NULL;
}

void Graph::preInit()
{
    Proxy::preInit();
    Hooks::preInit();
}
