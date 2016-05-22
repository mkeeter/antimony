#include <Python.h>

#include "undo/undo_add_node.h"

UndoAddNode::UndoAddNode(Node* node)
    : UndoDeleteNode(node), first(true)
{
    setText("'add node'");
}

void UndoAddNode::redo()
{
    // The first time redo is called, we've just finished dragging a
    // Node into place so all of the UI is already in place.
    if (first)
        first = false;
    else
        UndoDeleteNode::undo();
}

void UndoAddNode::undo()
{
    UndoDeleteNode::redo();
}
