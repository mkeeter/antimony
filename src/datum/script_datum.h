#ifndef SCRIPT_H
#define SCRIPT_H

#include <Python.h>

#include <QSet>
#include <QString>
#include <QMap>

#include "datum/eval.h"

class ScriptDatum : public EvalDatum
{
    Q_OBJECT
public:
    explicit ScriptDatum(QString name, QString expr, QObject *parent);
    virtual ~ScriptDatum();

    PyObject* makeInput(QString name, PyTypeObject* type);
    PyObject* makeOutput(QString name, PyObject* out);

    virtual PyTypeObject* getType() const override { return Py_None->ob_type; }
protected:
    /** Returns True if we can make an input or output datum with this name.
     */
    bool isValidName(QString name) const;

    /** Evaluates the script (getting back None) and causing datum creation.
     */
    virtual PyObject* getCurrentValue() override;

    /** Function that returns the desired start token for PyRun_String
     */
    virtual int getStartToken() const override;

    /** Function that modifies the globals dict before eval is called.
     */
    virtual void modifyGlobalsDict(PyObject* g) override;

    PyObject* globals;
    PyObject* input_func;
    PyObject* output_func;

    QSet<QString> touched;
};

#endif // SCRIPT_H
