#include <Python.h>

#include "graph/proxy/node.h"
#include "graph/proxy/graph.h"
#include "graph/proxy/datum.h"
#include "graph/proxy/script.h"
#include "graph/proxy/util.h"

#include "graph/node.h"

NodeProxy::NodeProxy(Node* n, GraphProxy* parent)
    : QObject(parent)
{
    n->installWatcher(this);
}

void NodeProxy::trigger(const NodeState& state)
{
    updateHash(state.datums, &datums, this);

    if (state.subgraph && !subgraph)
        subgraph = new GraphProxy(state.subgraph, this);
    else if (state.script && !script)
        script = new ScriptProxy(state.script, this);
}
