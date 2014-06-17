#ifndef SHAPE_DATUM_H
#define SHAPE_DATUM_H

#include <Python.h>
#include "datum/datum.h"
#include "cpp/fab.h"

class ShapeDatum : public Datum
{
    Q_OBJECT
public:
    explicit ShapeDatum(QString name, QObject* parent=0);

    /** Overload function to get the current value; always fails
     *  (because the shape will always from the input handler)
     */
    virtual PyObject* getCurrentValue() override { return NULL; }

    /** Returns the python Shape class.
     */
    virtual PyTypeObject* getType() const override { return fab::ShapeType; }
};

#endif // SHAPE_DATUM_H
