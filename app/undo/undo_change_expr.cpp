#include <Python.h>

#include <QTextCursor>
#include <QPlainTextEdit>
#include <QGraphicsTextItem>

#include "undo/undo_change_expr.h"

#include "graph/datum.h"

UndoChangeExpr::UndoChangeExpr(Datum* d, QString before,
                               QString after, QGraphicsTextItem* editor,
                               int cursor_before, int cursor_after)
    : datum(d), before(before), after(after), editor(editor),
      cursor_before(cursor_before), cursor_after(cursor_after)
{
    setText("'set value'");
}

void UndoChangeExpr::setCursor(int pos)
{
    if (editor)
    {
        QTextCursor c = editor->textCursor();
        c.setPosition(pos);
        editor->setTextCursor(c);
    }
}

void UndoChangeExpr::redo()
{
    datum->setText(after.toStdString());
    setCursor(cursor_after);
}

void UndoChangeExpr::undo()
{
    // Save text value and cursor position
    after = QString::fromStdString(datum->getText());
    if (editor)
        cursor_after = editor->textCursor().position();

    // Update the script's text and restore the cursor
    datum->setText(before.toStdString());
    setCursor(cursor_before);
}

void UndoChangeExpr::swapPointer(Datum* a, Datum* b) const
{
    if (datum == a)
        datum = b;
}
