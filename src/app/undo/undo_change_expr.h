#ifndef UNDO_CHANGE_EXPR_H
#define UNDO_CHANGE_EXPR_H

#include <Python.h>

#include <QString>
#include <QPointer>

#include "app/undo/undo_command.h"

class Datum;
class EvalDatum;

class UndoChangeExprCommand : public UndoCommand
{
public:
    UndoChangeExprCommand(EvalDatum* d, QString before, QString after);
    UndoChangeExprCommand(EvalDatum* d, QString before, QString after,
                          int cursor_before, int cursor_after,
                          QObject* obj);

    void redo() override;
    void undo() override;

    void swapDatum(Datum* a, Datum* b) const override;

protected:
    template<typename T> void _saveCursor();
    template<typename T> void _restoreCursor(int pos);

    // If obj is a QPlainTextEdit or a QGraphicsTextItem,
    // these functions will save cursor position (to cursor_after)
    // and restore cursor position (from the given argument)
    void saveCursor();
    void restoreCursor(int pos);

    mutable EvalDatum* d;
    QString before;
    QString after;

    int cursor_before;
    int cursor_after;
    QPointer<QObject> obj;
};


#endif
