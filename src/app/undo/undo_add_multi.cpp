#include "app/undo/undo_add_multi.h"

UndoAddMultiCommand::UndoAddMultiCommand(QSet<Node*> nodes,
                                         QSet<Link*> links,
                                         QString text)
    : UndoDeleteMultiCommand(nodes, links), first(true)
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
