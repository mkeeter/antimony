#include "undo/undo_command.h"

UndoCommand::UndoCommand(QUndoCommand* parent)
    : QUndoCommand(parent), stack(NULL)
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


