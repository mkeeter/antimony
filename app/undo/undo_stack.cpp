#include <Python.h>

#include "undo/undo_stack.h"
#include "undo/undo_command.h"

////////////////////////////////////////////////////////////////////////////////

UndoStack::UndoStack(QObject* parent)
    : QUndoStack(parent)
{
    // Nothing to do here
}

void UndoStack::push(UndoCommand* c)
{
    c->setStack(this);
    QUndoStack::push(c);
}
