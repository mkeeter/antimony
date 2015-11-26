#pragma once

#include <Python.h>

#include <QString>
#include <QPointer>
#include <QPlainTextEdit>

#include "undo/undo_command.h"

class ScriptNode;

class UndoChangeScript: public UndoCommand
{
public:
    /*
     *  Calls editor->undo() and editor->redo() to get the previous
     *  and current state of the script.
     */
    UndoChangeScript(ScriptNode* n, QPlainTextEdit* editor);

    void redo() override;
    void undo() override;

    void swapPointer(Node* a, Node* b) const override;

protected:
    /*
     *  If we have a valid QPlainTextEdit pointer,
     *  set its cursor to the given position.
     */
    void setCursor(int pos);

    mutable ScriptNode* node;
    QString before;
    QString after;

    /*
     *  Store a QPointer to the relevant text document.
     *
     *  If the window is closed, the pointer will be set to NULL;
     *  this needs to be checked whenever it is used.
     */
    QPointer<QPlainTextEdit> editor;
    int cursor_before;
    int cursor_after;
};
