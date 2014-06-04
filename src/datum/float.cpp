#include "float.h"

FloatDatum::FloatDatum(QString name, QString expr, QObject *parent)
    : EvalDatum(name, expr, parent)
{
    // Nothing to do here
}

bool FloatDatum::validate(PyObject *v) const
{
    return PyFloat_CheckExact(v);
}
