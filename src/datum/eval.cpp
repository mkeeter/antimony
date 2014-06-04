#include "datum/eval.h"

EvalDatum::EvalDatum(QString name, QString expr, QObject *parent) :
    Datum(name, parent)
{
    setExpr(expr);
}

PyObject* EvalDatum::getValue() const
{
    // python magic here!
    PyObject* new_value = PyRun_String(
                expr.toStdString().c_str(), 0, NULL, NULL);

    if (new_value != NULL && !validate(new_value))
    {
        Py_DECREF(new_value);
        new_value = NULL;
    }
    return new_value;
}

void EvalDatum::setExpr(QString new_expr)
{
    if (new_expr != expr)
    {
        expr = new_expr;
        update();
    }
}
