#include "app/undo/undo_delete_link.h"

#include "app/undo/stack.h"

#include "graph/datum/datum.h"
#include "graph/datum/link.h"

#include "ui/canvas/scene.h"

UndoDeleteLinkCommand::UndoDeleteLinkCommand(GraphScene* g, Link* link)
    : g(g), link(link), start(static_cast<Datum*>(link->parent())),
      end(link->getTarget())
{
    setText("'delete link'");
}

void UndoDeleteLinkCommand::redo()
{
    link->deleteLater();
}

void UndoDeleteLinkCommand::undo()
{
    auto new_link = start->linkFrom();
    Q_ASSERT(end->acceptsLink(new_link));
    end->addLink(new_link);

    g->makeUIfor(new_link);
    stack->swapPointer(link, new_link);
}

void UndoDeleteLinkCommand::swapDatum(Datum* a, Datum* b) const
{
    if (start == a)
        start = b;
    if (end == a)
        end = b;
}

void UndoDeleteLinkCommand::swapLink(Link* a, Link* b) const
{
    if (link == a)
        link = b;
}
