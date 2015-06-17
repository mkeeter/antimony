#ifndef UNDO_DELETE_MULTI_H
#define UNDO_DELETE_MULTI_H

#include <QSet>

#include "app/undo/undo_command.h"

class UndoDeleteMultiCommand : public UndoCommand
{
public:
    UndoDeleteMultiCommand(QSet<Node*> nodes, QSet<Link*> links,
                           QString text=QString());
};

#endif
