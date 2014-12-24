#ifndef FLOAT_OUTPUT_DATUM
#define FLOAT_OUTPUT_DATUM

#include "graph/datum/types/output_datum.h"

class FloatOutputDatum : public OutputDatum
{
    Q_OBJECT
public:
    explicit FloatOutputDatum(QString name, QObject* parent=0);
    PyTypeObject* getType() const override;
    DatumType::DatumType getDatumType() const override
        { return DatumType::FLOAT_OUTPUT; }

    QString getString() const override;
};

#endif
