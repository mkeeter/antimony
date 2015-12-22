#include <Python.h>

#include "undo/undo_delete_multi.h"

#include "graph/node.h"

UndoDeleteMulti::UndoDeleteMulti(QSet<Node*> nodes, QSet<Datum*> datums,
                                 QSet<QPair<const Datum*, Datum*>> links)
{
    setText("'delete'");

    for (auto n : nodes)
        for (auto d : n->childDatums())
            for (auto e : d->outgoingLinks())
                links.insert(QPair<Datum*, Datum*>(d, e));

    for (auto d : datums)
        for (auto e : d->outgoingLinks())
            links.insert(QPair<Datum*, Datum*>(d, e));

    /*
    for (auto k : links)
        new UndoDeleteLinkCommand(k.first, k.second, this);

    for (auto n : nodes)
        new UndoDeleteNodeCommand(n, this);
    */
}
