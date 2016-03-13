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
    CanvasInfo i;
    i.inspector[n] = after;
    App::instance()->getProxy()->setPositions(i);
}

void UndoMoveNode::undo()
{
    // FIXME: very inefficient
    CanvasInfo i;
    i.inspector[n] = before;
    App::instance()->getProxy()->setPositions(i);
}

void UndoMoveNode::swapPointer(Node* a, Node* b) const
{
    if (a == n)
        n = b;
}
