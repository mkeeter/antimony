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
    : QObject(parent), inspector(new InspectorFrame(n, parent->canvasScene()))
{
    n->installWatcher(this);

    // Automatically set inspector pointer to NULL on its deletion
    // (to prevent double-deletion in ~NodeProxy)
    connect(inspector, &QGraphicsObject::destroyed,
            [=]{ this->inspector = NULL; });
}

NodeProxy::~NodeProxy()
{
    if (inspector)
        delete inspector;
}

void NodeProxy::trigger(const NodeState& state)
{
    updateHash(state.datums, &datums, this);

    if (state.subgraph && !subgraph)
        subgraph = new GraphProxy(state.subgraph, this);
    else if (state.script && !script)
        script = new ScriptProxy(state.script, this);

    // Update inspector
    inspector->setNameValid(state.name_valid);
    if (state.script)
        inspector->setScriptValid(state.script->hasError());
}
