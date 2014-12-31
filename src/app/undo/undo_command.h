#ifndef UNDO_COMMAND_H
#define UNDO_COMMAND_H

#include <QUndoCommand>

class UndoStack;
class App;

class Node;
class Datum;
class Link;

class UndoCommand : public QUndoCommand
{
public:
    UndoCommand(QUndoCommand* parent=NULL);
    void setStack(UndoStack* s) { setStack(s, this); }
    void setApp(App* a) { setApp(a, this); }

    virtual void swapNode(Node*, Node*) const {}
    virtual void swapDatum(Datum*, Datum*) const {}
    virtual void swapLink(Link*, Link*) const {}

protected:
    static void setStack(UndoStack* s, const QUndoCommand* cmd);
    static void setApp(App* a, const QUndoCommand* cmd);

    // Backwards pointer to parent stack
    // Used so that commands can ask the stack to rewrite pointers
    // (e.g. when undoing Node deletion, any other commands in the stack
    //  that refer to that Node by pointer must be updated)
    mutable UndoStack* stack;

    // Pointer to parent App
    // (to access app-level function to create UIs for new objects)
    mutable App* app;
};

#endif
