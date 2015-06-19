#ifndef SHAPE_OUTPUT_DATUM
#define SHAPE_OUTPUT_DATUM

#include "graph/datum/types/output_datum.h"

class ShapeOutputDatum : public OutputDatum
{
    Q_OBJECT
public:
    explicit ShapeOutputDatum(QString name, Node* parent);
    PyTypeObject* getType() const override;
    DatumType::DatumType getDatumType() const override
        { return DatumType::SHAPE_OUTPUT; }

    static QString typeString() { return "shape output"; }
    QString getDatumTypeString() const override { return typeString(); }
};

#endif
