#include "app/undo/undo_delete_link.h"

UndoDeleteLinkCommand::UndoDeleteLinkCommand(GraphScene* g, Link* link)
    : UndoAddLinkCommand(g, link)
{
    first = false;
    setText("'delete link'");
}

void UndoDeleteLinkCommand::redo()
{
    UndoAddLinkCommand::undo();
}

void UndoDeleteLinkCommand::undo()
{
    UndoAddLinkCommand::redo();
}
