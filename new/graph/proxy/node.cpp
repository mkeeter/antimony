#include <Python.h>

#include "graph/proxy/node.h"
#include "graph/proxy/graph.h"
#include "graph/proxy/datum.h"
#include "graph/proxy/script.h"
#include "graph/proxy/util.h"

#include "canvas/inspector/frame.h"
#include "canvas/scene.h"

#include "graph/node.h"

NodeProxy::NodeProxy(Node* n, GraphProxy* parent)
    : QObject(parent), script(NULL), subgraph(NULL),
      inspector(new InspectorFrame(n, parent->canvasScene()))
{
    n->installWatcher(this);
    NULL_ON_DESTROYED(inspector);
}

NodeProxy::~NodeProxy()
{
    if (inspector)
        delete inspector;
}

void NodeProxy::trigger(const NodeState& state)
{
    if (state.subgraph && !subgraph)
        subgraph = new GraphProxy(state.subgraph, this);
    else if (state.script && !script)
        script = new ScriptProxy(state.script, this);

    updateHash(state.datums, &datums, this);
    {
        int i=0;
        for (auto d : state.datums)
            datums[d]->setIndex(i++);
    }

    // Update inspector
    inspector->setNameValid(state.name_valid);
    inspector->redoLayout();
}
