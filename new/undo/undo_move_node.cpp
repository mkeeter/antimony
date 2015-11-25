#include <Python.h>

#include "app/app.h"
#include "graph/proxy/graph.h"
#include "undo/undo_move_node.h"

UndoMoveNode::UndoMoveNode(Node* n, QPointF before, QPointF after)
    : n(n), before(before), after(after)
{
    setText("'move inspector'");
}

void UndoMoveNode::redo()
{
    // FIXME: very inefficient
    App::instance()->getProxy()->setInspectorPositions({{n, after}});
}

void UndoMoveNode::undo()
{
    // FIXME: very inefficient
    App::instance()->getProxy()->setInspectorPositions({{n, before}});
}

void UndoMoveNode::swapPointer(Node* a, Node* b) const
{
    if (a == n)
        n = b;
}
