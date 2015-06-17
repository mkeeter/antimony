#include "app/undo/undo_move.h"

#include "graph/node/node.h"

#include "ui/canvas/graph_scene.h"
#include "ui/canvas/inspector/inspector.h"

UndoMoveCommand::UndoMoveCommand(GraphScene* g, Node* n,
                                 QPointF before, QPointF after)
    : g(g), n(n), before(before), after(after)
{
    setText("'move inspector'");
}

void UndoMoveCommand::redo()
{
    g->getInspector(n)->setPos(after);
}

void UndoMoveCommand::undo()
{
    g->getInspector(n)->setPos(before);
}

void UndoMoveCommand::swapNode(Node* a, Node* b) const
{
    if (a == n)
        n = b;
}


