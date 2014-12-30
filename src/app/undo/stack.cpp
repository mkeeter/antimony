#include <Python.h>

#include "app/undo/stack.h"
#include "app/undo/undo_command.h"

UndoStack::UndoStack(QObject* parent)
    : QUndoStack(parent)
{
    // Nothing to do here
}

void UndoStack::swapPointer(Node* a, Node* b, const QUndoCommand* cmd)
{
    if (auto u = dynamic_cast<const UndoCommand*>(cmd))
        u->swapNode(a, b);
    for (int i=0; i < cmd->childCount(); ++i)
        swapPointer(a, b, cmd->child(i));
}

void UndoStack::swapPointer(Node* a, Node* b)
{
    for (int i=0; i < count(); ++i)
        swapPointer(a, b, command(i));
}

void UndoStack::swapPointer(Datum* a, Datum* b, const QUndoCommand* cmd)
{
    if (auto u = dynamic_cast<const UndoCommand*>(cmd))
        u->swapDatum(a, b);
    for (int i=0; i < cmd->childCount(); ++i)
        swapPointer(a, b, cmd->child(i));
}

void UndoStack::swapPointer(Datum* a, Datum* b)
{
    for (int i=0; i < count(); ++i)
        swapPointer(a, b, command(i));
}

void UndoStack::swapPointer(Link* a, Link* b, const QUndoCommand* cmd)
{
    if (auto u = dynamic_cast<const UndoCommand*>(cmd))
        u->swapLink(a, b);
    for (int i=0; i < cmd->childCount(); ++i)
        swapPointer(a, b, cmd->child(i));
}

void UndoStack::swapPointer(Link* a, Link* b)
{
    for (int i=0; i < count(); ++i)
        swapPointer(a, b, command(i));
}

void UndoStack::push(UndoCommand* c)
{
    c->setStack(this);
    QUndoStack::push(c);
}
