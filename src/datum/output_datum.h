#ifndef OUTPUT_DATUM_H
#define OUTPUT_DATUM_H

#include <Python.h>
#include "datum/datum.h"

class OutputDatum : public Datum
{
    Q_OBJECT
public:
    explicit OutputDatum(QString name, QObject* parent=0);
    ~OutputDatum();
    void setNewValue(PyObject* p);
    virtual QString getString() const override;

protected:
    /** Get the "current" value from new_value.
     */
    virtual PyObject* getCurrentValue() override;

    PyObject* new_value;
};

////////////////////////////////////////////////////////////////////////////////

class ShapeOutputDatum : public OutputDatum
{
    Q_OBJECT
public:
    explicit ShapeOutputDatum(QString name, QObject* parent=0);
    virtual PyTypeObject* getType() const override;
};

#endif // OUTPUT_DATUM_H
