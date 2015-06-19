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
#include "graph/datum/link.h"

#include "ui/canvas/graph_scene.h"

UndoDeleteNodeCommand::UndoDeleteNodeCommand(Node* n, QUndoCommand* parent)
    : UndoCommand(parent), n(n)
{
    setText("'delete node'");
}

void UndoDeleteNodeCommand::redo()
{
    // Find and save flat lists of all nodes and datums that will be deleted
    // (so that we can do a pointer swap when they're re-created)
    nodes = getNodes();
    datums = getDatums();

    // Serialize n into data byte array
    NodeRoot temp_root;
    n->setParent(&temp_root);
    data = app
        ? SceneSerializer(
                &temp_root, app->getGraphScene()->inspectorPositions()).run(
                SceneSerializer::SERIALIZE_NODES)
        : SceneSerializer(&temp_root).run(
                SceneSerializer::SERIALIZE_NODES);

    // Tell the system to delete the node
    n->deleteLater();
}

void UndoDeleteNodeCommand::undo()
{
    // Deserialize the saved node.
    NodeRoot temp_root;
    SceneDeserializer ds(&temp_root);
    ds.run(data);

    // Extract the node from the temporary root
    n = temp_root.findChild<Node*>();

    // Find the new lists of node and datum pointers
    auto new_nodes = getNodes();
    auto new_datums = getDatums();

    // Swap all the pointers!
    for (auto a = nodes.begin(), b = new_nodes.begin();
              a != nodes.end() && b != new_nodes.end(); ++a, ++b)
        stack->swapPointer(*a, *b);

    for (auto a = datums.begin(); a != datums.end(); ++a)
    {
        Q_ASSERT(new_datums.contains(a.key()));
        stack->swapPointer(a.value(), new_datums[a.key()]);
    }

    if (app)
    {
        app->makeUI(&temp_root);
        app->getGraphScene()->setInspectorPositions(ds.inspectors);
    }
}

QList<Node*> UndoDeleteNodeCommand::getNodes() const
{
    QList<Node*> nodes = n->findChildren<Node*>();
    nodes.prepend(n);
    return nodes;
}

QMap<QString, Datum*> UndoDeleteNodeCommand::getDatums() const
{
    QMap<QString, Datum*> out;
    for (auto d : n->findChildren<Datum*>())
        out[d->objectName()] = d;
    return out;
}

void UndoDeleteNodeCommand::swapNode(Node* a, Node* b) const
{
    if (n == a)
        n = b;
}
