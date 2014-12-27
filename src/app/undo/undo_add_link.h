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

protected:
    GraphScene* g;
    Datum* start;
    Datum* end;

    Link* link;

    bool first;
};
#endif
