#ifndef FLOAT_H
#define FLOAT_H

#include <Python.h>
#include "graph/datum/types/eval_datum.h"

class FloatDatum : public EvalDatum
{
    Q_OBJECT
public:
    explicit FloatDatum(QString name, Node* parent);
    explicit FloatDatum(QString name, QString expr, Node* parent);
    PyTypeObject* getType() const override { return &PyFloat_Type; }
    DatumType::DatumType getDatumType() const override
        { return DatumType::FLOAT; }

    static QString typeString() { return "float"; }
    QString getDatumTypeString() const override { return typeString(); }

    /*
     *  Attempts to add delta to the current value.
     *
     *  Will only succeed if the expression is a single float number.
     */
    bool dragValue(double delta);
};

#endif // FLOAT_H
