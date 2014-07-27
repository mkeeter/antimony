#include <Python.h>
#include "datum/float_datum.h"
#include "datum/input.h"
#include <QDebug>

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
