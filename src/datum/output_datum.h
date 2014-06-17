#ifndef OUTPUT_DATUM_H
#define OUTPUT_DATUM_H

#include <Python.h>
#include "datum/datum.h"

class OutputDatum : public Datum
{
    Q_OBJECT
public:
    explicit OutputDatum(QString name, QObject* parent=0);
    void setNewValue(PyObject* p);

protected:
    /** Get the "current" value from new_value.
     */
    virtual PyObject* getCurrentValue();

    PyObject* new_value;
};

////////////////////////////////////////////////////////////////////////////////

class ShapeOutputDatum : public OutputDatum
{
    Q_OBJECT
public:
    virtual PyTypeObject* getType() const;
};

#endif // OUTPUT_DATUM_H
