#ifndef INT_DATUM_H
#define INT_DATUM_H

#include <Python.h>
#include "graph/datum/types/eval_datum.h"

class IntDatum : public EvalDatum
{
    Q_OBJECT
public:
    explicit IntDatum(QString name, Node* parent);
    explicit IntDatum(QString name, QString expr, Node *parent);
    PyTypeObject* getType() const override { return &PyLong_Type; }
    DatumType::DatumType getDatumType() const override
        { return DatumType::INT; }
};

#endif // INT_DATUM_H
