#include <algorithm>

#include "graph/graph.h"
#include "graph/node.h"
#include "graph/proxy.h"

Graph::Graph(std::string n, Graph* parent)
    : name(n), parent(parent)
{
    // Nothing to do here
}

void Graph::install(Node* n)
{
    nodes.push_back(n);
}

PyObject* Graph::proxyDict(Node* locals, Downstream* caller)
{
    return Proxy::makeProxyFor(this, locals, caller);
}

PyObject* Graph::pyGetAttr(std::string name, Downstream* caller) const
{
    auto match = std::find_if(nodes.begin(), nodes.end(),
                              [&](Node* n){ return n->name == name; });
    if (match != nodes.end())
        return Proxy::makeProxyFor(*match, NULL, caller);
    else
        return NULL;
}
