#ifndef OUTPUT_DATUM_H
#define OUTPUT_DATUM_H

#include <Python.h>
#include "datum/datum.h"

class OutputDatum : public Datum
{
public:
    explicit OutputDatum(QString name, QObject* parent=0);
    ~OutputDatum();
    void setNewValue(PyObject* p);
    virtual QString getString() const override;
    bool canEdit() const override { return false; }
protected:
    /** Get the "current" value from new_value.
     */
    virtual PyObject* getCurrentValue() override;

    PyObject* new_value;
};

////////////////////////////////////////////////////////////////////////////////

class ShapeOutputDatum : public OutputDatum
{
public:
    explicit ShapeOutputDatum(QString name, QObject* parent=0);
    virtual PyTypeObject* getType() const override;
    DatumType::DatumType getDatumType() const override
        { return DatumType::SHAPE_OUTPUT; }
};

#endif // OUTPUT_DATUM_H
