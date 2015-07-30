#include "app/undo/undo_delete_link.h"
#include "app/undo/stack.h"

#include "graph/datum.h"

UndoDeleteLinkCommand::UndoDeleteLinkCommand(
        const Datum* start, Datum* end, QUndoCommand* parent)
    : UndoCommand(parent), start(start), end(end)
{
    setText("'delete link'");
}

void UndoDeleteLinkCommand::redo()
{
    end->uninstallLink(start);
}

void UndoDeleteLinkCommand::undo()
{
    end->installLink(start);
}

void UndoDeleteLinkCommand::swapDatum(Datum* a, Datum* b) const
{
    if (start == a)
        start = b;
    if (end == a)
        end = b;
}
