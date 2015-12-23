#include <Python.h>

#include "undo/undo_delete_link.h"
#include "undo/undo_delete_multi.h"

#include "graph/datum.h"

UndoDeleteLink::UndoDeleteLink(const Datum* source, Datum* target,
                               UndoDeleteMulti* parent)
    : UndoCommand(parent), source(source), target(target)
{
    setText("'delete link'");
}

////////////////////////////////////////////////////////////////////////////////

void UndoDeleteLink::undo()
{
    target->installLink(source);
}

void UndoDeleteLink::redo()
{
    target->uninstallLink(source);
}

////////////////////////////////////////////////////////////////////////////////

void UndoDeleteLink::swapPointer(Datum* a, Datum* b) const
{
    if (a == source)
        source = b;
    if (a == target)
        target = b;
}
