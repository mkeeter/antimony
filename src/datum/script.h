#ifndef SCRIPT_H
#define SCRIPT_H

#include <Python.h>

#include <string>
#include <QString>

#include "datum/eval.h"

class ScriptDatum : public EvalDatum
{
    Q_OBJECT
public:
    explicit ScriptDatum(QString name, QString expr, QObject *parent);
    virtual ~ScriptDatum();

    PyObject* makeInput(QString name, PyTypeObject* type);
    virtual PyTypeObject* getType() const { return Py_None->ob_type; }
protected:
    /** Function that returns the desired start token for PyRun_String
     */
    virtual int getStartToken() const;

    /** Function that modifies the globals dict before eval is called.
     */
    virtual void modifyGlobalsDict(PyObject* g);

    PyObject* globals;
    PyObject* input_func;
    PyObject* output_func;
};

#endif // SCRIPT_H
