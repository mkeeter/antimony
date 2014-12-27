#ifndef UNDO_ADD_LINK_H
#define UNDO_ADD_LINK_H

#include <Python.h>

#include "app/undo/undo_command.h"

class GraphScene;
class Datum;
class Link;

class UndoAddLinkCommand : public UndoCommand
{
public:
    UndoAddLinkCommand(GraphScene* g, Link* link);

    void redo() override;
    void undo() override;

    void swapDatum(Datum* a, Datum* b) const;
    void swapLink(Link* a, Link* b) const;

protected:
    GraphScene* g;

    mutable Link* link;

    mutable Datum* start;
    mutable Datum* end;

    bool first;
};
#endif
