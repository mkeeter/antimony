#pragma once

#include "undo/undo_delete_node.h"

class UndoAddNode : public UndoDeleteNode
{
public:
    UndoAddNode(Node* node);

    void redo() override;
    void undo() override;

protected:
    bool first;
};
