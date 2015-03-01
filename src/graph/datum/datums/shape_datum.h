#ifndef SHAPE_DATUM_H
#define SHAPE_DATUM_H

#include <Python.h>

#include "graph/datum/types/eval_datum.h"
#include "fab/fab.h"

class ShapeDatum : public EvalDatum
{
    Q_OBJECT
public:
    explicit ShapeDatum(QString name, Node* parent);

    PyTypeObject* getType() const override { return fab::ShapeType; }
    DatumType::DatumType getDatumType() const override
        { return DatumType::SHAPE; }
};

#endif // SHAPE_DATUM_H
