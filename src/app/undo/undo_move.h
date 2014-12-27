#ifndef UNDO_MOVE_H
#define UNDO_MOVE_H

#include <Python.h>

#include "app/undo/undo_command.h"

#include "graph/node/node.h"
#include "ui/canvas/scene.h"

class UndoMoveCommand : public UndoCommand
{
public:
    UndoMoveCommand(GraphScene* g, Node* n, QPointF a, QPointF b);

    void redo() override;
    void undo() override;

    void swapNode(Node* a, Node* b) const;

protected:
    QPointer<GraphScene> g;
    mutable QPointer<Node> n;
    QPointF a;
    QPointF b;
};

#endif
