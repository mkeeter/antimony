#ifndef UNDO_ADD_NODE_H
#define UNDO_ADD_NODE_H

#include "app/undo/undo_delete_node.h"

class UndoAddNodeCommand : public UndoDeleteNodeCommand
{
public:
    UndoAddNodeCommand(Node* node);

    void redo() override;
    void undo() override;
protected:
    bool first;
};

#endif
