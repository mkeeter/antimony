#ifndef SCRIPT_H
#define SCRIPT_H

#include <Python.h>
#include "datum/datum.h"

class ScriptDatum : public Datum
{
    Q_OBJECT
public:
    explicit ScriptDatum(QString name, QString expr, QObject *parent);
    void setScript(QString new_script);
    void makeInput(QString name, PyTypeObject* type);
    void makeOutput(QString name, PyObject* o);
protected:
    virtual PyObject* getCurrentValue();

    QString script;
    PyObject* globals;
};

#endif // SCRIPT_H
