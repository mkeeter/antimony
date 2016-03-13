#pragma once

#include <QUndoStack>

#include "undo/undo_command.h"

class Node;
class Datum;
class Graph;
class UndoCommand;

class UndoStack : public QUndoStack
{
public:
    UndoStack(QObject* parent=NULL);

    /*
     *  Performs a pointer swap for all commands in the stack
     */
    template <class T>
    void swapPointer(T* a, T* b)
    {
        for (int i=0; i < count(); ++i)
            swapPointer(a, b, command(i));
    }

    void push(UndoCommand* c);

protected:
    /*
     *  Performs a pointer swap for a specific command.
     *  (and recursively on its children)
     */
    template <class T>
    static void swapPointer(T* a, T* b, const QUndoCommand* cmd)
    {
        if (auto u = dynamic_cast<const UndoCommand*>(cmd))
            u->swapPointer(a, b);
        for (int i=0; i < cmd->childCount(); ++i)
            swapPointer(a, b, cmd->child(i));
    }
};
