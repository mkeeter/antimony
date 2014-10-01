#include <Python.h>

#include "datum/float_datum.h"
#include "datum/vec3_datum.h"


Vec3Datum::Vec3Datum(QString name, QObject* parent)
    : Datum(name, parent)
{
    // Nothing to do here
}

Vec3Datum::Vec3Datum(QString name, QString x, QString y, QString z,
                     QObject *parent)
    : Datum(name, parent)
{
    new FloatDatum("x", x, this);
    new FloatDatum("y", y, this);
    new FloatDatum("z", z, this);
}

QString Vec3Datum::getString() const
{
    return QString("(") + getDatum("x")->getString() + "," +
                          getDatum("y")->getString() + "," +
                          getDatum("x")->getString() + ")";
}

PyObject* Vec3Datum::getCurrentValue()
{
    return NULL;
}
