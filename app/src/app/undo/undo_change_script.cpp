#include <Python.h>

#include <QTextCursor>
#include <QPlainTextEdit>
#include <QGraphicsTextItem>

#include "app/undo/undo_change_script.h"

#include "graph/script_node.h"

UndoChangeScriptCommand::UndoChangeScriptCommand(
        ScriptNode* n, QString before, QString after)
    : UndoChangeScriptCommand(n, before, after, -1, -1, NULL)
{
    // Nothing to do here
}

UndoChangeScriptCommand::UndoChangeScriptCommand(
        ScriptNode* n, QString before, QString after,
        int cursor_before, int cursor_after,
        QPlainTextEdit* txt)
    : n(n), before(before), after(after),
      cursor_before(cursor_before), cursor_after(cursor_after),
      txt(txt)
{
    setText("'change script'");
}


void UndoChangeScriptCommand::saveCursor()
{
    if (txt)
        cursor_after = txt->textCursor().position();
}

void UndoChangeScriptCommand::restoreCursor(int pos)
{
    if (txt)
    {
        QTextCursor c = txt->textCursor();
        c.setPosition(pos);
        txt->setTextCursor(c);
    }
}

void UndoChangeScriptCommand::redo()
{
    n->setScript(after.toStdString());
    restoreCursor(cursor_after);
}

void UndoChangeScriptCommand::undo()
{
    // Save text value and cursor position
    saveCursor();
    after = QString::fromStdString(n->getScript());

    n->setScript(before.toStdString());

    // Restore cursor to previous position
    restoreCursor(cursor_before);
}

void UndoChangeScriptCommand::swapNode(Node* a, Node* b) const
{
    if (n == a)
    {
        assert(dynamic_cast<ScriptNode*>(b));
        n = static_cast<ScriptNode*>(b);
    }
}
