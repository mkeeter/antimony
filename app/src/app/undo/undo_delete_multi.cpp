#include <Python.h>

#include "app/undo/undo_delete_multi.h"
#include "app/undo/undo_delete_node.h"
#include "app/undo/undo_delete_link.h"
#include "graph/node.h"

UndoDeleteMultiCommand::UndoDeleteMultiCommand(
        QSet<Node*> nodes, QSet<QPair<const Datum*, Datum*>> links, QString text)
{
    setText(text.isNull() ? "'delete'" : text);

    for (auto n : nodes)
        for (auto d : n->childDatums())
            for (auto e : d->outgoingLinks())
                links.insert(QPair<Datum*, Datum*>(d, e));

    for (auto k : links)
        new UndoDeleteLinkCommand(k.first, k.second, this);

    for (auto n : nodes)
        new UndoDeleteNodeCommand(n, this);
}
