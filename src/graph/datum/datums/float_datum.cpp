#include <Python.h>

#include <QRegularExpression>

#include "graph/datum/datums/float_datum.h"
#include "graph/datum/input.h"

FloatDatum::FloatDatum(QString name, Node* parent)
    : EvalDatum(name, parent)
{
    input_handler = new SingleInputHandler(this);
}

FloatDatum::FloatDatum(QString name, QString expr, Node *parent)
    : FloatDatum(name, parent)
{
    setExpr(expr);
}

bool FloatDatum::dragValue(double delta)
{
    bool ok = false;

    QString s = getExpr();
    double v = s.toFloat(&ok);
    if (ok)
    {
        setExpr(QString::number(v + delta));
        return true;
    }

    QRegularExpression regex(
        "(.*[+\\-]\\s*)(\\d*(\\.\\d*|)(e\\d+(\\.\\d*|)|))"
    );
    auto match = regex.match(s);
    if (match.isValid())
    {
        v = match.captured(2).toFloat(&ok);
        if (ok)
            setExpr(match.captured(1) + QString::number(v + delta));
    }

    return ok;
}
