#include "graph/datum/datums/string_datum.h"
#include "graph/datum/input.h"

StringDatum::StringDatum(QString name, Node* parent)
    : EvalDatum(name, parent)
{
    input_handler = new SingleInputHandler(this);
}

StringDatum::StringDatum(QString name, QString expr, Node* parent)
    : StringDatum(name, parent)
{
    setExpr(expr);
}

QString StringDatum::prepareExpr(QString s) const
{
    return wrap ?  ("'" + s + "'") : s;
}

PyObject* StringDatum::getCurrentValue()
{
    PyObject* v;

    wrap = false;
    v = EvalDatum::getCurrentValue();
    if (v)
    {
        return v;
    }

    wrap = true;
    v = EvalDatum::getCurrentValue();
    return v;
}
