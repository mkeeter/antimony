#ifndef UNDO_CHANGE_EXPR_H
#define UNDO_CHANGE_EXPR_H

#include <Python.h>

#include <QString>

#include "app/undo/undo_command.h"

class Datum;
class EvalDatum;

class UndoChangeExprCommand : public UndoCommand
{
public:
    UndoChangeExprCommand(EvalDatum* d, QString before, QString after);

    void redo() override;
    void undo() override;

    void swapDatum(Datum* a, Datum* b) const;

protected:
    mutable EvalDatum* d;
    QString before;
    QString after;
};


#endif
