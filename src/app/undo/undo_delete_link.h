#ifndef UNDO_DELETE_LINK_H
#define UNDO_DELETE_LINK_H

#include <Python.h>

#include "app/undo/undo_command.h"

class GraphScene;
class Datum;
class Link;

class UndoDeleteLinkCommand : public UndoCommand
{
public:
    UndoDeleteLinkCommand(Link* link, QUndoCommand* parent=NULL);

    void redo() override;
    void undo() override;

    void swapDatum(Datum* a, Datum* b) const override;
    void swapLink(Link* a, Link* b) const override;

protected:
    GraphScene* g;

    mutable Link* link;

    mutable Datum* start;
    mutable Datum* end;
};
#endif
