#ifndef UNDO_DELETE_LINK_H
#define UNDO_DELETE_LINK_H

#include <Python.h>

#include "app/undo/undo_command.h"

class Datum;
class Link;

class UndoDeleteLinkCommand : public UndoCommand
{
public:
    UndoDeleteLinkCommand(const Datum* start, Datum* end,
                          QUndoCommand* parent=NULL);

    void redo() override;
    void undo() override;

    void swapDatum(Datum* a, Datum* b) const override;

protected:
    mutable const Datum* start;
    mutable Datum* end;
};
#endif
