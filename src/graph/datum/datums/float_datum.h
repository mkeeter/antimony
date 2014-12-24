#ifndef FLOAT_H
#define FLOAT_H

#include <Python.h>
#include "graph/datum/types/eval_datum.h"

class FloatDatum : public EvalDatum
{
    Q_OBJECT
public:
    explicit FloatDatum(QString name, QObject* parent=0);
    explicit FloatDatum(QString name, QString expr, QObject *parent = 0);
    PyTypeObject* getType() const override { return &PyFloat_Type; }
    DatumType::DatumType getDatumType() const override
        { return DatumType::FLOAT; }

    /*
     *  Attempts to add delta to the current value.
     *
     *  Will only succeed if the expression is a single float number.
     */
    bool dragValue(double delta);
};

#endif // FLOAT_H
