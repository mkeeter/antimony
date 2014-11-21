#include <Python.h>
#include "graph/datum/datums/float_datum.h"
#include "graph/datum/input.h"

FloatDatum::FloatDatum(QString name, QObject* parent)
    : EvalDatum(name, parent)
{
    input_handler = new SingleInputHandler(this);
}

FloatDatum::FloatDatum(QString name, QString expr, QObject *parent)
    : FloatDatum(name, parent)
{
    setExpr(expr);
}
