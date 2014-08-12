#ifndef EVAL_H
#define EVAL_H

#include <Python.h>
#include "datum/datum.h"

class EvalDatum : public Datum
{
    Q_OBJECT
public:
    explicit EvalDatum(QString name, QObject* parent=0);

    QString getExpr() const { return expr; }
    void setExpr(QString new_expr);
protected:
    /** Function called on the string before evaluation.
     */
    virtual QString prepareExpr(QString s) const;

    /** Overload function to actually get the current value.
     */
    PyObject* getCurrentValue() override;

    /** Returns a string suitable for display in a node viewer.
     */
    QString getString() const override;

    /** Validation function called on Python object.
     */
    virtual PyObject* validatePyObject(PyObject* v) const;

    /** Validation function called on expression before evalution.
     */
    virtual bool validateExpr(QString e) const;

    /** Validation function that checks object type and attempts to cast.
     */
    virtual PyObject* validateType(PyObject* v) const;

    /** Function that returns the desired start token for PyRun_String
     */
    virtual int getStartToken() const;

    /** Function that modifies the globals dict before eval is called.
     */
    virtual void modifyGlobalsDict(PyObject* g);

    /** Function called when there's a Python error.
     *  By default, prints the error.
     *  This function does not need to call PyErr_Clear().
     */
    virtual void onPyError();

    QString expr;
};

#endif // EVAL_H
