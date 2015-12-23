#include <Python.h>

#include "undo/undo_delete_datum.h"
#include "undo/undo_delete_multi.h"
#include "undo/undo_stack.h"

#include "app/app.h"
#include "graph/serialize/serializer.h"
#include "graph/serialize/deserializer.h"
#include "graph/proxy/graph.h"

#include "graph/datum.h"
#include "graph/node.h"

UndoDeleteDatum::UndoDeleteDatum(Datum* d, UndoDeleteMulti* parent)
    : UndoCommand(parent), d(d), n(d->parentNode())
{
    setText("'delete datum'");
}

UndoDeleteDatum::UndoDeleteDatum(Datum* d)
    : UndoDeleteDatum(d, NULL)
{
    // Nothing to do here
}

////////////////////////////////////////////////////////////////////////////////

void UndoDeleteDatum::swapPointer(Datum* a, Datum* b) const
{
    if (d == a)
        d = b;
}

void UndoDeleteDatum::swapPointer(Node* a, Node* b) const
{
    if (n == a)
        n = b;
}

////////////////////////////////////////////////////////////////////////////////

void UndoDeleteDatum::redo()
{
    // Serialize d into data byte array
    auto i = App::instance()->getProxy()->canvasInfo();
    data = SceneSerializer::serializeDatum(d, &i);

    // Tell the graph engine to delete the datum
    n->uninstall(d);
}

void UndoDeleteDatum::undo()
{
    // Deserialize the target datum into its parent node
    SceneDeserializer::Info ds;
    SceneDeserializer::deserializeDatum(data, n, &ds);

    // Extract the new datum pointer
    {
        auto d_new = n->childDatums().back();
        stack->swapPointer(d, d_new);
        d = d_new;
    }

    App::instance()->getProxy()->setPositions(ds.frames);
}

