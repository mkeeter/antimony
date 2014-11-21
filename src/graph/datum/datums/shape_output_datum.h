#ifndef SHAPE_OUTPUT_DATUM
#define SHAPE_OUTPUT_DATUM

#include "graph/datum/types/output_datum.h"

class ShapeOutputDatum : public OutputDatum
{
    Q_OBJECT
public:
    explicit ShapeOutputDatum(QString name, QObject* parent=0);
    PyTypeObject* getType() const override;
    DatumType::DatumType getDatumType() const override
        { return DatumType::SHAPE_OUTPUT; }
};

#endif
