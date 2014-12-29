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
    void setStack(UndoStack* s) { stack = s; }
    void setApp(App* a) { app = a; }

    virtual void swapNode(Node*, Node*) const {}
    virtual void swapDatum(Datum*, Datum*) const {}
    virtual void swapLink(Link*, Link*) const {}

protected:
    // Backwards pointer to parent stack
    // Used so that commands can ask the stack to rewrite pointers
    // (e.g. when undoing Node deletion, any other commands in the stack
    //  that refer to that Node by pointer must be updated)
    UndoStack* stack;

    // Pointer to parent App
    // (to access app-level function to create UIs for new objects)
    App* app;
};

#endif
