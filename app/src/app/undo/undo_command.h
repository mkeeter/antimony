#ifndef UNDO_COMMAND_H
#define UNDO_COMMAND_H

#include <QUndoCommand>

class UndoStack;

class Node;
class Datum;

class UndoCommand : public QUndoCommand
{
public:
    UndoCommand(QUndoCommand* parent=NULL);
    void setStack(UndoStack* s) { setStack(s, this); }

    virtual void swapNode(Node*, Node*) const {}
    virtual void swapDatum(Datum*, Datum*) const {}

protected:
    static void setStack(UndoStack* s, const QUndoCommand* cmd);

    // Backwards pointer to parent stack
    // Used so that commands can ask the stack to rewrite pointers
    // (e.g. when undoing Node deletion, any other commands in the stack
    //  that refer to that Node by pointer must be updated)
    mutable UndoStack* stack;
};

#endif
