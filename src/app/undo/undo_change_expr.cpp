#include "app/undo/undo_change_expr.h"

#include "graph/datum/types/eval_datum.h"
#include "graph/datum/datum.h"


UndoChangeExprCommand::UndoChangeExprCommand(EvalDatum* d, QString before,
                                             QString after)
    : d(d), before(before), after(after)
{
    setText("'set value'");
}

void UndoChangeExprCommand::redo()
{
    d->setExpr(after);
}

void UndoChangeExprCommand::undo()
{
    d->setExpr(before);
}

void UndoChangeExprCommand::swapDatum(Datum* a, Datum* b) const
{
    Q_ASSERT(dynamic_cast<EvalDatum*>(b));
    if (d == a)
        d = static_cast<EvalDatum*>(b);
}
