#ifndef FLOAT_H
#define FLOAT_H

#include <Python.h>
#include "datum/eval.h"

class FloatDatum : public EvalDatum
{
    Q_OBJECT
public:
    explicit FloatDatum(QString name, QString expr, QObject *parent = 0);
    virtual PyTypeObject* getType() const { return &PyFloat_Type; }
protected:
    virtual bool validate(PyObject* v) const;

};

#endif // FLOAT_H
