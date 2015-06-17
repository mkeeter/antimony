#include <Python.h>

#include "app/undo/undo_delete_multi.h"
#include "app/undo/undo_delete_node.h"
#include "app/undo/undo_delete_link.h"
#include "graph/node/node.h"

UndoDeleteMultiCommand::UndoDeleteMultiCommand(QSet<Node*> nodes,
                                               QSet<Link*> links,
                                               QString text)
{
    setText(text.isNull() ? "'delete'" : text);

    for (auto n : nodes)
        links.unite(n->getLinks());

    for (auto k : links)
        new UndoDeleteLinkCommand(k, this);

    for (auto n : nodes)
        new UndoDeleteNodeCommand(n, this);
}
