#ifndef OUTPUT_DATUM_H
#define OUTPUT_DATUM_H

#include <Python.h>
#include "graph/datum/datum.h"

class OutputDatum : public Datum
{
    Q_OBJECT
public:
    explicit OutputDatum(QString name, Node* parent);
    ~OutputDatum();
    void setNewValue(PyObject* p);
    QString getString() const override;
    bool canEdit() const override { return false; }
protected:
    /** Get the "current" value from new_value.
     */
    PyObject* getCurrentValue() override;

    PyObject* new_value;
};

#endif // OUTPUT_DATUM_H
