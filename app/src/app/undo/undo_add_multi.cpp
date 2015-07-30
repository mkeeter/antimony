#include "app/undo/undo_add_multi.h"

UndoAddMultiCommand::UndoAddMultiCommand(QSet<Node*> nodes,
                                         QString text)
    : UndoDeleteMultiCommand(nodes, {}), first(true)
{
    setText(text.isNull() ? "'add node'" : text);
}

void UndoAddMultiCommand::redo()
{
    // The first time redo is called, we've just creating stuff,
    // so don't redo anything.
    if (first)
        first = false;
    else
        UndoDeleteMultiCommand::undo();
}

void UndoAddMultiCommand::undo()
{
    UndoDeleteMultiCommand::redo();
}
