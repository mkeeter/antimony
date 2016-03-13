#pragma once

#include "undo/undo_delete_datum.h"

class UndoAddDatum : public UndoDeleteDatum
{
public:
    UndoAddDatum(Datum* d);

    void redo() override;
    void undo() override;

protected:
    bool first;
};
