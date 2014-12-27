#include <Python.h>

#include "app/undo/stack.h"
#include "app/undo/undo_move.h"

UndoStack::UndoStack(QObject* parent)
    : QUndoStack(parent)
{
    // Nothing to do here
}


void UndoStack::swapPointer(Node* a, Node* b)
{
    for (int i=0; i < count(); ++i)
    {
        auto c = command(i);
        if (auto u = dynamic_cast<const UndoMoveCommand*>(c))
            u->swapNode(a, b);
    }
}

void UndoStack::swapPointer(Datum* a, Datum* b)
{
}

void UndoStack::swapPointer(Link* a, Link* b)
{
}
