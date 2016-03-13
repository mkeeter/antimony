#include "undo/undo_command.h"

class UndoDeleteMulti;

class UndoDeleteLink : public UndoCommand
{
public:
    UndoDeleteLink(const Datum* source, Datum* target,
                   UndoDeleteMulti* parent);

    /*
     *  Actual commands to execute undo and redo
     */
    void redo() override;
    void undo() override;

    /*
     *  Node and graph pointer swap
     */
    void swapPointer(Datum* a, Datum* b) const override;

protected:
    mutable const Datum* source;
    mutable Datum* target;
};
