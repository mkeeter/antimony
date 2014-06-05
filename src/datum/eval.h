#ifndef EVAL_H
#define EVAL_H

#include <Python.h>
#include "datum/datum.h"


class EvalDatum : public Datum
{
    Q_OBJECT
public:
    explicit EvalDatum(QString name, QObject* parent=0);

protected:
    virtual PyObject* getValue() const;
    virtual bool validate(PyObject* v) const=0;
    void setExpr(QString new_expr);

    QString expr;

    friend class TestDatum;
};

#endif // EVAL_H
