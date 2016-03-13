#pragma once

#include <QSet>

#include "undo/undo_command.h"

class Node;
class Datum;

class UndoDeleteMulti : public UndoCommand
{
public:
    UndoDeleteMulti(QSet<Node*> nodes, QSet<Datum*> datums,
                    QSet<QPair<const Datum*, Datum*>> links);
};
