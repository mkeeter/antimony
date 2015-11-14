#include <Python.h>

#include "graph/proxy/graph.h"
#include "graph/proxy/node.h"
#include "graph/proxy/superdatum.h"

#include "graph/proxy/util.h"

#include "graph/graph.h"

GraphProxy::GraphProxy(Graph* g, QObject* parent)
    : QObject(parent)
{
    g->installWatcher(this);
}

void GraphProxy::trigger(const GraphState& state)
{
    updateHash(state.nodes,  &nodes,  this);
    updateHash(state.datums, &datums, this);
}
