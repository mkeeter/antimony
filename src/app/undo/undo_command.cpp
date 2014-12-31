#include "app/undo/undo_command.h"

UndoCommand::UndoCommand(QUndoCommand* parent)
    : QUndoCommand(parent), stack(NULL), app(NULL)
{
    // Nothing to do here
}

void UndoCommand::setStack(UndoStack* s, const QUndoCommand* cmd)
{
    if (auto u = dynamic_cast<const UndoCommand*>(cmd))
        u->stack = s;

    for (int i=0; i < cmd->childCount(); ++i)
        setStack(s, cmd->child(i));
}


void UndoCommand::setApp(App* a, const QUndoCommand* cmd)
{
    if (auto u = dynamic_cast<const UndoCommand*>(cmd))
        u->app = a;

    for (int i=0; i < cmd->childCount(); ++i)
        setApp(a, cmd->child(i));
}
