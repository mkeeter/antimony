#include <algorithm>

#include "graph/graph.h"
#include "graph/node.h"
#include "graph/proxy.h"

Graph::Graph(std::string n, Graph* parent)
    : name(n), uid(0), parent(parent)
{
    // Nothing to do here
}

uint32_t Graph::install(Node* n)
{
    // Find the lowest unused unique ID number
    std::unordered_set<uint32_t> indices;
    std::for_each(nodes.begin(), nodes.end(),
                  [&](const std::unique_ptr<Node>& n)
                  { indices.insert(n->uid); });
    uint32_t uid = 0;
    while (indices.find(uid) != indices.end())
        uid++;

    nodes.push_back(std::unique_ptr<Node>(n));
    return uid;
}

PyObject* Graph::proxyDict(Node* locals, Downstream* caller)
{
    return Proxy::makeProxyFor(this, locals, caller);
}

PyObject* Graph::pyGetAttr(std::string name, Downstream* caller) const
{
    auto match = std::find_if(nodes.begin(), nodes.end(),
                              [&](const std::unique_ptr<Node>& n)
                              { return n->name == name; });
    if (match != nodes.end())
        return Proxy::makeProxyFor(match->get(), NULL, caller);
    else
        return NULL;
}
