#ifndef SCRIPT_DATUM_H
#define SCRIPT_DATUM_H

#include <Python.h>

#include <QSet>
#include <QString>

#include "datum/eval.h"

class ScriptDatum : public EvalDatum
{
    Q_OBJECT
public:
    explicit ScriptDatum(QString name, QObject *parent);
    explicit ScriptDatum(QString name, QString expr, QObject *parent);
    virtual ~ScriptDatum();

    PyObject* makeInput(QString name, PyTypeObject* type);
    PyObject* makeOutput(QString name, PyObject* out);

    PyTypeObject* getType() const override { return Py_None->ob_type; }
    bool hasOutput() const override { return false; }

    /** Returns the line on which an error occurred or -1.
     */
    int getErrorLine() const { return error_lineno; }

    /** Returns the error type or an empty string.
     */
    QString getErrorType() const { return error_type; }

    DatumType::DatumType getDatumType() const override
        { return DatumType::SCRIPT; }

signals:
    void datumsChanged();

protected:

    /** Returns true if we are already evaluating this datum.
     */
    bool isRecursing() const override { return globals; }

    /** Returns True if we can make an input or output datum with this name.
     */
    bool isValidName(QString name) const;

    /** Evaluates the script (getting back None) and causing datum creation.
     */
    PyObject* getCurrentValue() override;

    /** Function that returns the desired start token for PyRun_String
     */
    int getStartToken() const override;

    /** Function that modifies the globals dict before eval is called.
     */
    void modifyGlobalsDict(PyObject* g) override;

    /** On Python error, record line number and traceback.
     */
    void onPyError() override;

    PyObject* globals;
    PyObject* input_func;
    PyObject* output_func;

    QSet<QString> touched;
    bool datums_changed;

    int error_lineno;
    QString error_type;
};

#endif // SCRIPT_DATUM_H
