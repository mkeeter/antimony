#include <algorithm>

#include "graph/graph.h"
#include "graph/node.h"
#include "graph/proxy.h"

Graph::Graph(std::string n, Graph* parent)
    : name(n), uid(0), parent(parent)
{
    // Nothing to do here
}

PyObject* Graph::proxyDict(Node* locals, Downstream* caller)
{
    return Proxy::makeProxyFor(this, locals, caller);
}

PyObject* Graph::pyGetAttr(std::string name, Downstream* caller) const
{
    auto m = get(name, nodes);
    return m ? Proxy::makeProxyFor(m, NULL, caller) : NULL;
}
