#ifndef FUNCTION_DATUM_H
#define FUNCTION_DATUM_H

#include <Python.h>
#include <QObject>

#include "datum/datum.h"
#include "cpp/fab.h"

class FunctionDatum : public Datum
{
    Q_OBJECT
public:
    explicit FunctionDatum(QString name, QObject* parent);

    explicit FunctionDatum(QString name, QObject* parent,
                           QString f, QList<QString> args);

    virtual ~FunctionDatum();

    /** User editing is always forbidden.
     */
    bool canEdit() const override { return false; }

    QString getString() const override;

    QString getFunctionName() const { return function_name; }
    QList<QString> getArguments() const { return arguments; }

    void setFunctionName(QString f);
    void setArguments(QList<QString> args);

protected:
    /** Must return a new reference to a module containing the target function.
     */
    virtual PyObject* getModule() const=0;

    /** Call the desired function using parent datums as arguments.
     */
    PyObject* getCurrentValue() override;

    QString function_name;
    QList<QString> arguments;
    PyObject* function;
};

////////////////////////////////////////////////////////////////////////////////

class ShapeFunctionDatum : public FunctionDatum
{
    Q_OBJECT
public:
    explicit ShapeFunctionDatum(QString name, QObject* parent);
    explicit ShapeFunctionDatum(QString name, QObject* parent,
                                QString func_name, QList<QString> args);
    DatumType::DatumType getDatumType() const override
        { return DatumType::SHAPE_FUNCTION; }
protected:
    PyObject* getModule() const override;
    PyTypeObject* getType() const override { return fab::ShapeType; }
};

#endif // FUNCTION_DATUM_H
