#include <Python.h>
#include "graph/datum/datums/int_datum.h"
#include "graph/datum/input.h"

IntDatum::IntDatum(QString name, Node* parent)
    : EvalDatum(name, parent)
{
    input_handler = new SingleInputHandler(this);
}

IntDatum::IntDatum(QString name, QString expr, Node *parent)
    : IntDatum(name, parent)
{
    setExpr(expr);
}

bool IntDatum::dragValue(int delta)
{
    bool ok = false;

    QString s = getExpr();
    int i = s.toInt(&ok);
    if (ok)
        setExpr(QString::number(i + delta));
    return ok;
}

