#pragma once

#include <QUndoStack>

class Node;
class Datum;
class Graph;
class UndoCommand;

class UndoStack : public QUndoStack
{
public:
    UndoStack(QObject* parent=NULL);

    template <class T>
    void swapPointer(T* a, T* b);

    /*
     *  Performs a pointer swap for all commands in the stack
     */
    void swapPointer(Node* a, Node* b);
    void swapPointer(Datum* a, Datum* b);
    void swapPointer(Graph* a, Graph* b);

    void push(UndoCommand* c);

protected:
    /*
     *  Performs a pointer swap for a specific command.
     *  (and recursively on its children)
     */
    template <class T>
    static void swapPointer(T* a, T* b, const QUndoCommand* cmd);

    static void swapPointer(Node* a, Node* b, const QUndoCommand* cmd);
    static void swapPointer(Datum* a, Datum* b, const QUndoCommand* cmd);
    static void swapPointer(Graph* a, Graph* b, const QUndoCommand* cmd);
};
