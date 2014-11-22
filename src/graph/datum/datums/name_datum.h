#ifndef NAME_H
#define NAME_H

#include <Python.h>
#include "graph/datum/types/eval_datum.h"

class NameDatum : public EvalDatum
{
    Q_OBJECT
public:
    explicit NameDatum(QString name, QObject *parent = 0);
    explicit NameDatum(QString name, QString expr, QObject *parent = 0);
    PyTypeObject* getType() const override { return &PyUnicode_Type; }
    bool hasOutput() const override { return false; }
    DatumType::DatumType getDatumType() const override
        { return DatumType::NAME; }

signals:
    void nameChanged(QString new_name);
public slots:
    void onNameChange();
protected:
    QString prepareExpr(QString s) const override;
    PyObject* validatePyObject(PyObject* v) const override;
    bool validateExpr(QString e) const override;

    /** Checks if the given object collides with a Python keyword.
     */
    static bool isKeyword(PyObject* v);

    static PyObject* kwlist_contains;
};

#endif // NAME_H
