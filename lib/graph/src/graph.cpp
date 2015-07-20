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

PyObject* Graph::proxyDict(Datum* caller)
{
    return Proxy::makeProxyFor(this, caller);
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
    return Root::install(n, &nodes);
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

void Graph::loadScriptHooks(PyObject* g, Node* n)
{
    if (external)
        external->loadScriptHooks(g, n);
}

std::list<Node*> Graph::childNodes() const
{
    std::list<Node*> out;
    for (const auto& ptr : nodes)
        out.push_back(ptr.get());
    return out;
}

void Graph::loadDatumHooks(PyObject* g)
{
    if (external)
        external->loadDatumHooks(g);
}

PyObject* Graph::pyGetAttr(std::string name, Downstream* caller) const
{
    auto m = get(name, nodes);
    return m ? Proxy::makeProxyFor(m, caller) : NULL;
}

void Graph::preInit()
{
    Proxy::preInit();
    Hooks::preInit();
}
