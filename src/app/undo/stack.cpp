#include <Python.h>

#include "app/undo/stack.h"
#include "app/undo/undo_command.h"

UndoStack::UndoStack(QObject* parent)
    : QUndoStack(parent)
{
    // Nothing to do here
}


void UndoStack::swapPointer(Node* a, Node* b)
{
    for (int i=0; i < count(); ++i)
        static_cast<const UndoCommand*>(command(i))->swapNode(a, b);
}

void UndoStack::swapPointer(Datum* a, Datum* b)
{
    for (int i=0; i < count(); ++i)
        static_cast<const UndoCommand*>(command(i))->swapDatum(a, b);
}

void UndoStack::swapPointer(Link* a, Link* b)
{
    for (int i=0; i < count(); ++i)
        static_cast<const UndoCommand*>(command(i))->swapLink(a, b);
}

void UndoStack::push(UndoCommand* c)
{
    c->setStack(this);
    QUndoStack::push(c);
}
