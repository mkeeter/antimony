#ifndef FLOAT_OUTPUT_DATUM
#define FLOAT_OUTPUT_DATUM

#include "graph/datum/types/output_datum.h"

class FloatOutputDatum : public OutputDatum
{
    Q_OBJECT
public:
    explicit FloatOutputDatum(QString name, Node* parent);
    PyTypeObject* getType() const override;
    DatumType::DatumType getDatumType() const override
        { return DatumType::FLOAT_OUTPUT; }

    static QString typeString() { return "float output"; }
    QString getDatumTypeString() const override { return typeString(); }

    QString getString() const override;
};

#endif
