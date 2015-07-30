#ifndef UNDO_CHANGE_EXPR_H
#define UNDO_CHANGE_EXPR_H

#include <Python.h>

#include <QString>
#include <QPointer>
#include <QPlainTextEdit>

#include "app/undo/undo_command.h"

class Node;

class UndoChangeScriptCommand : public UndoCommand
{
public:
    UndoChangeScriptCommand(Node* n, QString before, QString after);
    UndoChangeScriptCommand(Node* n, QString before, QString after,
                            int cursor_before, int cursor_after,
                            QPlainTextEdit* txt);

    void redo() override;
    void undo() override;

    void swapNode(Node* a, Node* b) const override;

protected:
    void saveCursor();
    void restoreCursor(int pos);

    mutable Node* n;
    QString before;
    QString after;

    int cursor_before;
    int cursor_after;
    QPointer<QPlainTextEdit> txt;
};


#endif
