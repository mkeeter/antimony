#ifndef UNDO_STACK
#define UNDO_STACK

#include <QUndoStack>

class Node;
class Datum;
class Link;
class UndoCommand;

class UndoStack : public QUndoStack
{
public:
    UndoStack(QObject* parent=NULL);

    void swapPointer(Node* a, Node* b);
    void swapPointer(Datum* a, Datum* b);
    void swapPointer(Link* a, Link* b);

    void push(UndoCommand* c);
};

#endif
