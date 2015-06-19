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

    static QString typeString() { return "int"; }
    QString getDatumTypeString() const override { return typeString(); }

    /*
     *  Attempts to add delta to the current value.
     *
     *  Will only succeed if the expression is a single int number.
     */
    bool dragValue(int delta);
};

#endif // INT_DATUM_H
