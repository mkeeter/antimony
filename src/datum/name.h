#ifndef NAME_H
#define NAME_H

#include <Python.h>
#include "datum/eval.h"

class NameDatum : public EvalDatum
{
    Q_OBJECT
public:
    explicit NameDatum(QString name, QString expr, QObject *parent = 0);
    virtual PyTypeObject* getType() const { return &PyUnicode_Type; }
protected:
    virtual bool validate(PyObject* v) const;
    static bool isKeyword(PyObject* v);

    static PyObject* kwlist_contains;
};

#endif // NAME_H
