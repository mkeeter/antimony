#ifndef VEC3_DATUM_H
#define VEC3_DATUM_H

#include <Python.h>
#include "datum/eval_datum.h"
#include "fab/fab.h"

class Vec3Datum : public Datum
{
    Q_OBJECT
public:
    explicit Vec3Datum(QString name, QObject* parent=0);
    explicit Vec3Datum(QString name, QString x, QString y, QString z,
                       QObject *parent = 0);

    QString getString() const override;
    bool canEdit() const override { return false; }

    PyTypeObject* getType() const override { return fab::Vec3Type; }
    DatumType::DatumType getDatumType() const override
        { return DatumType::VEC3; }

protected:
    bool areChildrenValid() const;

    PyObject* getCurrentValue();
};

#endif // FLOAT_H
