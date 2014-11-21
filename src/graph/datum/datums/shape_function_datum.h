#ifndef SHAPE_FUNCTION_DATUM
#define SHAPE_FUNCTION_DATUM

#include "graph/datum/types/function_datum.h"

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
    PyTypeObject* getType() const override;
};

#endif
