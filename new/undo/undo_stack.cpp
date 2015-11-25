#include <Python.h>

#include "undo/undo_stack.h"
#include "undo/undo_command.h"

////////////////////////////////////////////////////////////////////////////////

UndoStack::UndoStack(QObject* parent)
    : QUndoStack(parent)
{
    // Nothing to do here
}

template <class T>
void UndoStack::swapPointer(T* a, T* b)
{
    for (int i=0; i < count(); ++i)
        swapPointer(a, b, command(i));
}

void UndoStack::swapPointer(Node* a, Node* b)
{
    swapPointer(a, b);
}

void UndoStack::swapPointer(Datum* a, Datum* b)
{
    swapPointer(a, b);
}

void UndoStack::swapPointer(Graph* a, Graph* b)
{
    swapPointer(a, b);
}

void UndoStack::push(UndoCommand* c)
{
    c->setStack(this);
    QUndoStack::push(c);
}

////////////////////////////////////////////////////////////////////////////////

template <class T>
void UndoStack::swapPointer(T* a, T* b, const QUndoCommand* cmd)
{
    if (auto u = dynamic_cast<const UndoCommand*>(cmd))
        u->swapPointer(a, b);
    for (int i=0; i < cmd->childCount(); ++i)
        swapPointer(a, b, cmd->child(i));
}

void UndoStack::swapPointer(Datum* a, Datum* b, const QUndoCommand* cmd)
{
    swapPointer(a, b, cmd);
}

void UndoStack::swapPointer(Node* a, Node* b, const QUndoCommand* cmd)
{
    swapPointer(a, b, cmd);
}

void UndoStack::swapPointer(Graph* a, Graph* b, const QUndoCommand* cmd)
{
    swapPointer(a, b, cmd);
}
