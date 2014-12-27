#ifndef UNDO_COMMAND_H
#define UNDO_COMMAND_H

#include <QUndoCommand>

class UndoStack;

class UndoCommand : public QUndoCommand
{
public:
    void setStack(UndoStack* s) { stack = s; }
protected:
    UndoStack* stack;
};

#endif
