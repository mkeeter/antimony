#include <Python.h>
#include "datum/int_datum.h"
#include "datum/input.h"
#include <QDebug>

IntDatum::IntDatum(QString name, QObject* parent)
    : EvalDatum(name, parent)
{
    input_handler = new SingleInputHandler(this);
}

IntDatum::IntDatum(QString name, QString expr, QObject *parent)
    : IntDatum(name, parent)
{
    setExpr(expr);
}
