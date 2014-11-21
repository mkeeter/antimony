#ifndef SHAPE_INPUT_DATUM_H
#define SHAPE_INPUT_DATUM_H

#include <Python.h>
#include "graph/datum/types/input_datum.h"

class ShapeInputDatum : public InputDatum
{
    Q_OBJECT
public:
    explicit ShapeInputDatum(QString name, QObject* parent=0);

    PyTypeObject* getType() const override;

    DatumType::DatumType getDatumType() const override
        { return DatumType::SHAPE_INPUT; }
};

#endif // SHAPE_INPUT_DATUM_H
