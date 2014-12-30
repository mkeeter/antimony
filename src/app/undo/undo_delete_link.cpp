#include "app/undo/undo_delete_link.h"

#include "app/undo/stack.h"

#include "graph/datum/datum.h"
#include "graph/datum/link.h"

#include "app/app.h"

UndoDeleteLinkCommand::UndoDeleteLinkCommand(Link* link, QUndoCommand* parent)
    : UndoCommand(parent), link(link),
      start(static_cast<Datum*>(link->parent())), end(link->getTarget())
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

    if (app)
        app->newLink(new_link);

    // Swap link pointer for command in the stack
    // (including this one)
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
