#include <Python.h>

#include "app/app.h"
#include "graph/proxy/graph.h"
#include "undo/undo_move_datum.h"

UndoMoveDatum::UndoMoveDatum(Datum* d, QPointF before, QPointF after)
    : d(d), before(before), after(after)
{
    setText("'move inspector'");
}

void UndoMoveDatum::redo()
{
    // FIXME: very inefficient
    CanvasInfo i;
    i.subdatum[d] = after;
    App::instance()->getProxy()->setPositions(i);
}

void UndoMoveDatum::undo()
{
    // FIXME: very inefficient
    CanvasInfo i;
    i.subdatum[d] = before;
    App::instance()->getProxy()->setPositions(i);
}

void UndoMoveDatum::swapPointer(Datum* a, Datum* b) const
{
    if (a == d)
        d = b;
}

