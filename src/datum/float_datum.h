#ifndef FLOAT_H
#define FLOAT_H

#include <Python.h>
#include "datum/eval.h"

class FloatDatum : public EvalDatum
{
public:
    explicit FloatDatum(QString name, QObject* parent=0);
    explicit FloatDatum(QString name, QString expr, QObject *parent = 0);
    virtual PyTypeObject* getType() const override { return &PyFloat_Type; }
    DatumType::DatumType getDatumType() const override
        { return DatumType::FLOAT; }
};

#endif // FLOAT_H
