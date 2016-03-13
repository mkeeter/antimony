#pragma once

#include <QList>
#include <QMap>
#include <QJsonObject>

#include "undo/undo_command.h"

class UndoDeleteMulti;

class UndoDeleteNode : public UndoCommand
{
public:
    /*
     *  UndoDeleteNode constructor can only be called with a parent
     *  UndoDeleteMulti object, as deleting nodes needs to be done
     *  along-side deleting their links.
     */
    UndoDeleteNode(Node* n, UndoDeleteMulti* parent);

    /*
     *  Actual commands to execute undo and redo
     */
    void redo() override;
    void undo() override;

    /*
     *  Node and graph pointer swap
     */
    void swapPointer(Node* a, Node* b) const override;
    void swapPointer(Graph* a, Graph* b) const override;

protected:
    /*
     *  Protected constructor used in UndoAddNode subclass
     */
    UndoDeleteNode(Node* n);

    struct ChildPointers
    {
        QList<Datum*> datums;
        QList<Node*> nodes;
        QList<Graph*> graphs;
    };

    /*
     *  Walk child datums and nodes in a deterministic order
     */
    ChildPointers walkChildren() const;

    /*
     *  Calls stack->swapPointer from every a to every b
     */
    template <class T>
    void pointerSwapList(QList<T*> as, QList<T*> bs);

    /*  Pointer to the target node and its parent graph */
    mutable Node* n;
    mutable Graph* g;

    /*  This object stores the serialized node  */
    QJsonObject data;

    /*  Pointers to datums and nodes (if this is a GraphNode) stored within   *
     *  the target node.  These lists are generated in a deterministic order  *
     *  and used for pointer swaps on undo / redo.                            */
    ChildPointers children;
};
