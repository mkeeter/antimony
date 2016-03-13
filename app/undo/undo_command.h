#pragma once

#include <QUndoCommand>

class UndoStack;

class Node;
class Datum;
class Graph;

class UndoCommand : public QUndoCommand
{
public:
    UndoCommand(QUndoCommand* parent=NULL);

    /*
     *  Sets the internal 'stack' pointer for this command
     *  and recursively for child commands
     */
    void setStack(UndoStack* s) { setStack(s, this); }

    /*
     *  Functions to be overloaded by derived classes
     */
    virtual void swapPointer(Node*, Node*) const {}
    virtual void swapPointer(Datum*, Datum*) const {}
    virtual void swapPointer(Graph*, Graph*) const {}

protected:
    /*
     *  If the given command is an UndoCommand, set its stack pointer;
     *  recurse over all children in any case.
     */
    static void setStack(UndoStack* s, const QUndoCommand* cmd);

    /*
     *  Backwards pointer to parent stack
     *
     *  Used so that commands can ask the stack to rewrite pointers
     *  (e.g. when undoing Node deletion, any other commands in the stack
     *   that refer to that Node by pointer must be updated)
     */
    mutable UndoStack* stack;
};
