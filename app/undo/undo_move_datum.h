#pragma once

#include <QPointF>
#include "undo/undo_command.h"

class UndoMoveDatum : public UndoCommand
{
public:
    UndoMoveDatum(Datum* d, QPointF before, QPointF after);

    void redo() override;
    void undo() override;

    void swapPointer(Datum* a, Datum* b) const override;

protected:
    mutable Datum* d;

    QPointF before;
    QPointF after;
};
