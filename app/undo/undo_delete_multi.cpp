#include <Python.h>

#include "undo/undo_delete_multi.h"
#include "undo/undo_delete_node.h"
#include "undo/undo_delete_datum.h"
#include "undo/undo_delete_link.h"

#include "graph/node.h"

#include <QDebug>
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

    for (auto k : links)
        new UndoDeleteLink(k.first, k.second, this);

    for (auto n : nodes)
        new UndoDeleteNode(n, this);

    for (auto d : datums)
        new UndoDeleteDatum(d, this);
}
