#include <Python.h>

#include "app/undo/undo_delete_node.h"
#include "app/undo/undo_delete_link.h"
#include "app/undo/stack.h"

#include "app/app.h"

#include "graph/node/node.h"
#include "graph/node/root.h"
#include "graph/node/serializer.h"
#include "graph/node/deserializer.h"
#include "graph/datum/datum.h"

#include "ui/canvas/scene.h"

UndoDeleteNodeCommand::UndoDeleteNodeCommand(
        Node* n, GraphScene* g, QSet<Link*> ignore)
    : n(n), g(g)
{
    setText("'delete node'");

    for (auto k : n->getLinks())
        if (!ignore.contains(k))
            new UndoDeleteLinkCommand(k, this);
}

void UndoDeleteNodeCommand::redo()
{
    // Find and save flat lists of all nodes and datums that will be deleted
    // (so that we can do a pointer swap when they're re-created)
    nodes = getNodes();
    datums = getDatums();

    // Save the root so we know where to re-create this node.
    root = static_cast<NodeRoot*>(n->parent());

    // Serialize n into data byte array
    NodeRoot temp_root;
    n->setParent(&temp_root);
    data = g
        ? SceneSerializer(&temp_root, g->inspectorPositions()).run()
        : SceneSerializer(&temp_root).run();

    // Tell the system to delete the node
    n->deleteLater();

    // Call redo to redo all link deletion
    // (not strictly necessary, but nice for symmetry reasons)
    UndoCommand::redo();
}

void UndoDeleteNodeCommand::undo()
{
    // Deserialize the saved node.
    NodeRoot temp_root;
    SceneDeserializer ds(&temp_root);
    ds.run(data);

    // Extract the node from the temporary root
    n = temp_root.findChild<Node*>();
    n->setParent(root);
    if (g)
        g->setInspectorPositions(ds.inspectors);

    // Find the new lists of node and datum pointers
    auto new_nodes = getNodes();
    auto new_datums = getDatums();

    // Swap all the pointers!
    for (auto a = nodes.begin(), b = new_nodes.begin();
            a != nodes.end() && b != new_nodes.end(); ++a, ++b)
        stack->swapPointer(*a, *b);

    for (auto a = datums.begin(), b = new_datums.begin();
            a != datums.end() && b != new_datums.end(); ++a, ++b)
        stack->swapPointer(*a, *b);

    if (app)
        app->newNode(n);

    // Call undo to undo all child commands (re-creating deleted Links)
    UndoCommand::undo();
}

QList<Node*> UndoDeleteNodeCommand::getNodes() const
{
    QList<Node*> nodes = n->findChildren<Node*>();
    nodes.prepend(n);
    return nodes;
}

QList<Datum*> UndoDeleteNodeCommand::getDatums() const
{
    return n->findChildren<Datum*>();
}

void UndoDeleteNodeCommand::swapNode(Node* a, Node* b) const
{
    if (n == a)
        n = b;
}
