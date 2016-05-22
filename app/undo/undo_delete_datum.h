#pragma once

#include <QJsonObject>

#include "undo/undo_command.h"

class UndoDeleteMulti;
class GraphNode;

class UndoDeleteDatum : public UndoCommand
{
public:
    /*
     *  UndoDeleteDatum constructor can only be called with a parent
     *  UndoDeleteMulti object, as deleting datums needs to be done
     *  along-side deleting their links.
     */
    UndoDeleteDatum(Datum* d, UndoDeleteMulti* parent);

    /*
     *  Actual commands to execute undo and redo
     */
    void redo() override;
    void undo() override;

    /*
     *  Node and graph pointer swap
     */
    void swapPointer(Datum* a, Datum* b) const override;
    void swapPointer(Node* a, Node* b) const override;

protected:
    /*
     *  Protected constructor used in UndoAddDatum subclass
     */
    UndoDeleteDatum(Datum* d);

    /*  Pointer to the target datum and its parent node */
    mutable Datum* d;
    mutable GraphNode* n;

    /*  This object stores the serialized datum  */
    QJsonObject data;
};
