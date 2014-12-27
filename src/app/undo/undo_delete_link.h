#ifndef UNDO_DELETE_LINK_H
#define UNDO_DELETE_LINK_H

#include <Python.h>

#include "app/undo/undo_add_link.h"

class UndoDeleteLinkCommand : public UndoAddLinkCommand
{
public:
    UndoDeleteLinkCommand(GraphScene* g, Link* link);

    void redo() override;
    void undo() override;
};
#endif
