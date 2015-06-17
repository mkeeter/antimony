#ifndef STRING_DATUM_H
#define STRING_DATUM_H

#include "graph/datum/types/eval_datum.h"

class StringDatum : public EvalDatum
{
public:
    explicit StringDatum(QString name, Node* parent);
    explicit StringDatum(QString name, QString expr, Node* parent);

    PyTypeObject* getType() const override { return &PyUnicode_Type; }
    DatumType::DatumType getDatumType() const override
        { return DatumType::STRING; }

    static QString typeString() { return "string"; }
    QString getDatumTypeString() const override { return typeString(); }

protected:
    QString prepareExpr(QString s) const override;
    PyObject* getCurrentValue() override;

    bool wrap;
};

#endif
