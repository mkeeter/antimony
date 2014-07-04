#ifndef NAME_H
#define NAME_H

#include <Python.h>
#include "datum/eval.h"

class NameDatum : public EvalDatum
{
    Q_OBJECT
public:
    explicit NameDatum(QString name, QString expr, QObject *parent = 0);
    virtual PyTypeObject* getType() const override { return &PyUnicode_Type; }
    virtual bool hasOutput() const override { return false; }
signals:
    void nameChanged(QString new_name);
public slots:
    void onNameChange();
protected:
    virtual QString prepareExpr(QString s) const override;
    virtual PyObject* validatePyObject(PyObject* v) const override;
    virtual bool validateExpr(QString e) const override;

    /** Checks if the given object collides with a Python keyword.
     */
    static bool isKeyword(PyObject* v);

    static PyObject* kwlist_contains;
};

#endif // NAME_H
