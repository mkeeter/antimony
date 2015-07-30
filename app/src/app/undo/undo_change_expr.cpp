#include <Python.h>

#include <QTextCursor>
#include <QPlainTextEdit>
#include <QGraphicsTextItem>

#include "app/undo/undo_change_expr.h"

#include "graph/datum.h"

UndoChangeExprCommand::UndoChangeExprCommand(
        Datum* d, QString before, QString after)
    : UndoChangeExprCommand(d, before, after, -1, -1, NULL)
{
    // Nothing to do here
}

UndoChangeExprCommand::UndoChangeExprCommand(
        Datum* d, QString before, QString after,
        int cursor_before, int cursor_after,
        QGraphicsTextItem* txt)
    : d(d), before(before), after(after),
      cursor_before(cursor_before), cursor_after(cursor_after),
      txt(txt)
{
    setText("'set value'");
}


void UndoChangeExprCommand::saveCursor()
{
    if (txt)
        cursor_after = txt->textCursor().position();
}

void UndoChangeExprCommand::restoreCursor(int pos)
{
    if (txt)
    {
        QTextCursor c = txt->textCursor();
        c.setPosition(pos);
        txt->setTextCursor(c);
    }
}

void UndoChangeExprCommand::redo()
{
    d->setText(after.toStdString());
    restoreCursor(cursor_after);
}

void UndoChangeExprCommand::undo()
{
    // Save text value and cursor position
    saveCursor();
    after = QString::fromStdString(d->getText());

    d->setText(before.toStdString());

    // Restore cursor to previous position
    restoreCursor(cursor_before);
}

void UndoChangeExprCommand::swapDatum(Datum* a, Datum* b) const
{
    if (d == a)
        d = b;
}
