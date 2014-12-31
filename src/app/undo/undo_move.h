#ifndef UNDO_MOVE_H
#define UNDO_MOVE_H

#include <Python.h>

#include <QPointF>

#include "app/undo/undo_command.h"

class GraphScene;
class Node;

class UndoMoveCommand : public UndoCommand
{
public:
    UndoMoveCommand(GraphScene* g, Node* n, QPointF before, QPointF after);

    void redo() override;
    void undo() override;

    void swapNode(Node* a, Node* b) const override;

protected:
    GraphScene* g;
    mutable Node* n;
    QPointF before;
    QPointF after;
};

#endif
