#ifndef FLOAT_H
#define FLOAT_H

#include <Python.h>
#include "datum/eval.h"

class FloatDatum : public EvalDatum
{
    Q_OBJECT
public:
    explicit FloatDatum(QString name, QString expr, QObject *parent = 0);
    virtual ~FloatDatum() { /* Nothing to do here */ }

protected:
    virtual bool validate(PyObject* v) const;

};

#endif // FLOAT_H
