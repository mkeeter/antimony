#include <Python.h>

#include "undo/undo_stack.h"
#include "undo/undo_delete_node.h"
#include "undo/undo_delete_multi.h"

#include "app/app.h"
#include "graph/serialize/serializer.h"
#include "graph/serialize/deserializer.h"
#include "graph/proxy/graph.h"

#include "graph/node.h"
#include "graph/graph_node.h"
#include "graph/graph.h"

UndoDeleteNode::UndoDeleteNode(Node* n, UndoDeleteMulti* parent)
    : UndoCommand(parent), n(n), g(n->parentGraph())
{
    setText("'delete node'");
}

UndoDeleteNode::UndoDeleteNode(Node* n)
    : UndoDeleteNode(n, NULL)
{
    // Nothing to do here
}

////////////////////////////////////////////////////////////////////////////////

template <class T>
void UndoDeleteNode::pointerSwapList(QList<T*> as, QList<T*> bs)
{
    auto a = as.begin();
    auto b = bs.begin();
    while (a != as.end())
    {
        stack->swapPointer(*(a++), *(b++));
    }
    Q_ASSERT(b == bs.end());
}

////////////////////////////////////////////////////////////////////////////////

void UndoDeleteNode::swapPointer(Node* a, Node* b) const
{
    if (n == a)
        n = b;
}

void UndoDeleteNode::swapPointer(Graph* a, Graph* b) const
{
    if (g == a)
        g = b;
}

////////////////////////////////////////////////////////////////////////////////

void UndoDeleteNode::redo()
{
    // Find and save all children so we can swap pointers on undo
    children = walkChildren();

    // Serialize n into data byte array
    auto i = App::instance()->getProxy()->canvasInfo();
    data = SceneSerializer::serializeNode(n, &i);

    // Tell the system to delete the node
    g->uninstall(n);
}

void UndoDeleteNode::undo()
{
    // Deserialize the saved node
    SceneDeserializer::Info ds;
    SceneDeserializer::deserializeNode(data, g, &ds);

    // Extract the new node pointer
    n = g->childNodes().back();

    // Find all of the new child pointers
    auto new_children = walkChildren();

    // Swap all of the pointers
    pointerSwapList(children.nodes, new_children.nodes);
    pointerSwapList(children.datums, new_children.datums);
    pointerSwapList(children.graphs, new_children.graphs);

    App::instance()->getProxy()->setPositions(ds.frames);
}

////////////////////////////////////////////////////////////////////////////////

UndoDeleteNode::ChildPointers UndoDeleteNode::walkChildren() const
{
    QList<Node*> targets = {n};
    ChildPointers out;

    while (targets.length())
    {
        auto t = targets.front();
        targets.pop_front();

        // Store this node's datums
        for (auto d : t->childDatums())
            out.datums.push_back(d);

        // Store the node pointer itself
        out.nodes.push_back(t);

        // If this is a graph node, store the graph pointer and recurse
        if (auto gn = dynamic_cast<GraphNode*>(t))
        {
            out.graphs.push_back(gn->getGraph());
            for (auto c : gn->getGraph()->childNodes())
                targets.push_back(c);
        }
    }

    return out;
}
