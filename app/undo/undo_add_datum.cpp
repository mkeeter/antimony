#include <Python.h>

#include "undo/undo_add_datum.h"

UndoAddDatum::UndoAddDatum(Datum* datum)
    : UndoDeleteDatum(datum), first(true)
{
    setText("'add datum'");
}

void UndoAddDatum::redo()
{
    // The first time redo is called, we've just finished dragging a
    // Datum into place so all of the UI is already in place.
    if (first)
        first = false;
    else
        UndoDeleteDatum::undo();
}

void UndoAddDatum::undo()
{
    UndoDeleteDatum::redo();
}

