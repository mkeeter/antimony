#ifndef UNDO_DELETE_NODE_H
#define UNDO_DELETE_NODE_H

#include <QSet>
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
     *  ignore is a set of links to ignore in the "find links that will
     *  be-deleted" stage (because they're already scheduled for deletion,
     *  and while double-deletion isn't a problem, double-re-creation is).
     */
    UndoDeleteNodeCommand(Node* n, GraphScene* g,
                          QSet<Link*> ignore=QSet<Link*>());

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
