#pragma once

#include <QPointF>
#include "undo/undo_command.h"

class UndoMoveNode : public UndoCommand
{
public:
    UndoMoveNode(Node* n, QPointF before, QPointF after);

    void redo() override;
    void undo() override;

    void swapPointer(Node* a, Node* b) const override;

protected:
    mutable Node* n;

    QPointF before;
    QPointF after;
};
