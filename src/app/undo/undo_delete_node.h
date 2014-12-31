#ifndef UNDO_DELETE_NODE_H
#define UNDO_DELETE_NODE_H

#include <QList>

#include "app/undo/undo_command.h"

class GraphScene;
class NodeRoot;

class UndoDeleteNodeCommand : public UndoCommand
{
public:
    /*
     *  n is the target node to delete
     *  g is the GraphScene in which to look up an inspector position
     *      (or NULL)
     */
    UndoDeleteNodeCommand(Node* n, GraphScene* g);

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

    GraphScene* g;
    QByteArray data;
};

#endif
