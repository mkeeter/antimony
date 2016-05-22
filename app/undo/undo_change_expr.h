#pragma once

#include <Python.h>

#include <QString>
#include <QPointer>
#include <QGraphicsTextItem>

#include "undo/undo_command.h"

class Datum;

class UndoChangeExpr: public UndoCommand
{
public:
    /*
     *  Constructs an undo command that reverses a change to datum
     */
    UndoChangeExpr(Datum* d, QString before, QString after,
                   QGraphicsTextItem* editor=NULL,
                   int cursor_before=0, int cursor_after=0);

    void redo() override;
    void undo() override;

    void swapPointer(Datum* a, Datum* b) const override;

protected:
    /*
     *  If we have a valid QGraphicsTextItem pointer,
     *  set its cursor to the given position.
     */
    void setCursor(int pos);

    mutable Datum* datum;
    QString before;
    QString after;

    /*
     *  Store a QPointer to the relevant text document.
     *
     *  If the window is closed, the pointer will be set to NULL;
     *  this needs to be checked whenever it is used.
     */
    QPointer<QGraphicsTextItem> editor;
    int cursor_before;
    int cursor_after;
};
