#include <Python.h>
#include <QTextCursor>

#include "app/undo/undo_change_expr.h"

#include "graph/datum/types/eval_datum.h"
#include "graph/datum/datum.h"

UndoChangeExprCommand::UndoChangeExprCommand(
        EvalDatum* d, QString before, QString after)
    : UndoChangeExprCommand(d, before, after, -1, -1, NULL)
{
    // Nothing to do here
}

UndoChangeExprCommand::UndoChangeExprCommand(
        EvalDatum* d, QString before, QString after,
        int cursor_before, int cursor_after,
        QPlainTextEdit* doc)
    : d(d), before(before), after(after),
      cursor_before(cursor_before), cursor_after(cursor_after),
      doc(doc)
{
    setText("'set value'");
}

void UndoChangeExprCommand::redo()
{
    d->setExpr(after);
    if (doc)
    {
        QTextCursor c = doc->textCursor();
        c.setPosition(cursor_after);
        doc->setTextCursor(c);
    }
}

void UndoChangeExprCommand::undo()
{
    // Save text value and cursor position
    if (doc)
        cursor_after = doc->textCursor().position();
    after = d->getExpr();

    d->setExpr(before);

    // Restore cursor to previous position
    if (doc)
    {
        QTextCursor c = doc->textCursor();
        c.setPosition(cursor_before);
        doc->setTextCursor(c);
    }
}

void UndoChangeExprCommand::swapDatum(Datum* a, Datum* b) const
{
    Q_ASSERT(dynamic_cast<EvalDatum*>(b));
    if (d == a)
        d = static_cast<EvalDatum*>(b);
}
