#include "app/undo/undo_move.h"

#include "graph/node/node.h"

#include "ui/canvas/scene.h"
#include "ui/canvas/inspector/inspector.h"

UndoMoveCommand::UndoMoveCommand(GraphScene* g, Node* n, QPointF a, QPointF b)
    : g(g), n(n), a(a), b(b)
{
    setText("'move inspector'");
}

void UndoMoveCommand::redo()
{
    g->getInspector(n)->setPos(b);
}

void UndoMoveCommand::undo()
{
    g->getInspector(n)->setPos(a);
}

void UndoMoveCommand::swapNode(Node* a, Node* b) const
{
    if (a == n)
        n = b;
}


