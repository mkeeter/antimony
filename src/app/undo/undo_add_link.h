#ifndef UNDO_ADD_LINK_H
#define UNDO_ADD_LINK_H

#include <Python.h>

#include "app/undo/undo_command.h"

#include "graph/datum/datum.h"
#include "graph/datum/link.h"

#include "ui/canvas/scene.h"

class UndoAddLinkCommand : public UndoCommand
{
public:
    UndoAddLinkCommand(GraphScene* g, Link* link);

    void redo() override;
    void undo() override;

    void swapDatum(Datum* a, Datum* b) const;

protected:
    QPointer<GraphScene> g;
    mutable QPointer<Datum> start;
    mutable QPointer<Datum> end;

    QPointer<Link> link;

    bool first;
};
#endif
