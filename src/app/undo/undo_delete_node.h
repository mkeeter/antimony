#ifndef UNDO_DELETE_NODE_H
#define UNDO_DELETE_NODE_H

#include <QList>

#include "app/undo/undo_command.h"

class NodeRoot;

class UndoDeleteNodeCommand : public UndoCommand
{
public:
    /*
     *  n is the target node to delete
     */
    UndoDeleteNodeCommand(Node* n, QString text=QString());

    void redo();
    void undo();

    void swapNode(Node* a, Node* b) const override;

protected:
    QList<Node*> getNodes() const;
    QList<Datum*> getDatums() const;

    mutable Node* n;

    NodeRoot* root;
    QList<Node*> nodes;
    QList<Datum*> datums;

    QByteArray data;
};

#endif
