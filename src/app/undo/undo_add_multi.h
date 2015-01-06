#ifndef UNDO_ADD_MULTI_H
#define UNDO_ADD_MULTI_H

#include "app/undo/undo_delete_multi.h"

class UndoAddMultiCommand : public UndoDeleteMultiCommand
{
public:
    UndoAddMultiCommand(QSet<Node*> nodes,
                        QSet<Link*> links,
                        QString text=QString());

    void redo() override;
    void undo() override;
protected:
    bool first;
};

#endif
