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
    UndoChangeExprCommand(Datum* d, QString before, QString after);
    UndoChangeExprCommand(Datum* d, QString before, QString after,
                          int cursor_before, int cursor_after,
                          QGraphicsTextItem* obj);

    void redo() override;
    void undo() override;

    void swapDatum(Datum* a, Datum* b) const override;

protected:
    void saveCursor();
    void restoreCursor(int pos);

    mutable Datum* d;
    QString before;
    QString after;

    int cursor_before;
    int cursor_after;
    QPointer<QGraphicsTextItem> txt;
};


#endif
