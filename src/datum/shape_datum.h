#ifndef SHAPE_DATUM_H
#define SHAPE_DATUM_H

#include <Python.h>
#include "datum/datum.h"
#include "cpp/fab.h"

class ShapeDatum : public Datum
{
public:
    explicit ShapeDatum(QString name, QObject* parent=0);

    /** Returns the python Shape class.
     */
    virtual PyTypeObject* getType() const override { return fab::ShapeType; }

    /** User editing is always forbidden.
     */
    virtual bool canEdit() const override { return false; }

    virtual QString getString() const override;

    DatumType::DatumType getDatumType() const override
        { return DatumType::SHAPE; }

    bool hasOutput() const override { return false; }

protected:
    /** Overload function to get the current value; always fails
     *  (because the shape will always come from the input handler)
     */
    virtual PyObject* getCurrentValue() override { return NULL; }

};

#endif // SHAPE_DATUM_H
