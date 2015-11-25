#include <Python.h>

#include "graph/proxy/node.h"
#include "graph/proxy/graph.h"
#include "graph/proxy/datum.h"
#include "graph/proxy/script.h"
#include "graph/proxy/util.h"

#include "canvas/inspector/frame.h"
#include "canvas/scene.h"

#include "graph/node.h"
#include "graph/graph_node.h"
#include "graph/script_node.h"

NodeProxy::NodeProxy(Node* n, GraphProxy* parent)
    : QObject(parent), node(n), script(NULL), subgraph(NULL),
      inspector(new InspectorFrame(n, parent->canvasScene()))
{
    if (auto graph_node = dynamic_cast<GraphNode*>(n))
        subgraph = new GraphProxy(graph_node->getGraph(), this);
    else if (auto script_node = dynamic_cast<ScriptNode*>(n))
        script = new ScriptProxy(script_node->getScriptPointer(), this);
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
    updateHash(state.datums, &datums, this);

    {   // Set indices of datums for proper sorting
        int i=0;
        for (auto d : state.datums)
            datums[d]->setIndex(i++);
    }

    // Update inspector
    inspector->setNameValid(state.name_valid);
    inspector->redoLayout();
}

////////////////////////////////////////////////////////////////////////////////

QMap<Node*, QPointF> NodeProxy::inspectorPositions() const
{
    QMap<Node*, QPointF> out;
    out[node] = inspector->pos();

    if (subgraph)
        out.unite(subgraph->inspectorPositions());

    return out;
}

void NodeProxy::setInspectorPositions(const QMap<Node*, QPointF>& pos)
{
    if (pos.contains(node))
        inspector->setPos(pos[node]);

    if (subgraph)
        subgraph->setInspectorPositions(pos);
}
