#include "app/undo/undo_add_link.h"
#include "app/undo/stack.h"

#include "graph/datum/datum.h"
#include "graph/datum/link.h"

#include "ui/canvas/scene.h"

UndoAddLinkCommand::UndoAddLinkCommand(GraphScene* g, Link* link)
    : g(g), link(link), start(static_cast<Datum*>(link->parent())),
      end(link->getTarget()), first(true)
{
    setText("'add link'");
}

void UndoAddLinkCommand::redo()
{
    // The first time redo is called, we've just finished
    // dragging a connection into place so all of the UI
    // is already in place.
    if (first)
    {
        first = false;
    }
    else
    {
        auto new_link = start->linkFrom();
        Q_ASSERT(end->acceptsLink(new_link));
        end->addLink(new_link);

        g->makeUIfor(new_link);
        stack->swapPointer(link, new_link);
    }
}

void UndoAddLinkCommand::undo()
{
    link->deleteLater();
}

