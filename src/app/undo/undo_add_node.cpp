#include "app/undo/undo_add_node.h"

UndoAddNodeCommand::UndoAddNodeCommand(Node* node, QString text)
    : UndoDeleteNodeCommand(node), first(true)
{
    if (text.isNull())
        setText("'add node'");
    else
        setText(text);
}

void UndoAddNodeCommand::redo()
{
    // The first time redo is called, we've just finished dragging a
    // Node into place so all of the UI is already in place.
    if (first)
        first = false;
    else
        UndoDeleteNodeCommand::undo();
}

void UndoAddNodeCommand::undo()
{
    UndoDeleteNodeCommand::redo();
}
