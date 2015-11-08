#include <Python.h>
#include "graph/graph_node.h"
#include "graph/graph.h"

GraphNode::GraphNode(std::string name, Graph* root)
    : Node(name, root), subgraph(new Graph(this))
{
    // Nothing to do here
}

GraphNode::GraphNode(std::string name, uint32_t uid, Graph* root)
    : Node(name, uid, root), subgraph(new Graph(this))
{
    // Nothing to do here
}

bool GraphNode::makeDatum(std::string name, PyTypeObject* type,
                          std::string value, bool output)
{
}
