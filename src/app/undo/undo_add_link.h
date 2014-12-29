#ifndef UNDO_ADD_LINK_H
#define UNDO_ADD_LINK_H

#include <Python.h>

#include "app/undo/undo_delete_link.h"

class UndoAddLinkCommand : public UndoDeleteLinkCommand
{
public:
    UndoAddLinkCommand(Link* link);

    void redo() override;
    void undo() override;

protected:
    bool first;
};

#endif
