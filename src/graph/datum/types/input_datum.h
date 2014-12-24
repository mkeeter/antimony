#ifndef INPUT_DATUM_H
#define INPUT_DATUM_H

#include <Python.h>
#include "graph/datum/datum.h"

class InputDatum : public Datum
{
    Q_OBJECT
public:
    explicit InputDatum(QString name, Node* parent);

    /** User editing is always forbidden.
     */
    bool canEdit() const override { return false; }

    QString getString() const override;
    bool hasOutput() const override { return false; }

protected:
    /** Overload function to get the current value; always fails
     *  (because the shape will always come from the input handler)
     */
    PyObject* getCurrentValue() override { return NULL; }
};

#endif // INPUT_DATUM_H
