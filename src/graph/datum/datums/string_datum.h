#ifndef STRING_DATUM_H
#define STRING_DATUM_H

#include "graph/datum/types/eval_datum.h"

class StringDatum : public EvalDatum
{
public:
    explicit StringDatum(QString name, QObject* parent=0);
    explicit StringDatum(QString name, QString expr, QObject* parent=0);

    PyTypeObject* getType() const override { return &PyUnicode_Type; }
    DatumType::DatumType getDatumType() const override
        { return DatumType::STRING; }

protected:
    QString prepareExpr(QString s) const override;
    PyObject* getCurrentValue() override;

    bool wrap;
};

#endif
