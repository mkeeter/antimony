#include <Python.h>

#include "datum/float_datum.h"
#include "datum/vec3_datum.h"
#include "datum/input.h"

Vec3Datum::Vec3Datum(QString name, QObject* parent)
    : Datum(name, parent)
{
    input_handler = new SingleInputHandler(this);
}

Vec3Datum::Vec3Datum(QString name, QString x, QString y, QString z,
                     QObject *parent)
    : Vec3Datum(name, parent)
{
    new FloatDatum("x", x, this);
    new FloatDatum("y", y, this);
    new FloatDatum("z", z, this);
    update();
}

QString Vec3Datum::getString() const
{
    return QString("(") + getDatum("x")->getString() + "," +
                          getDatum("y")->getString() + "," +
                          getDatum("x")->getString() + ")";
}

bool Vec3Datum::areChildrenValid() const
{
    return findChild<FloatDatum*>("x")->getValid() &&
           findChild<FloatDatum*>("y")->getValid() &&
           findChild<FloatDatum*>("z")->getValid();
}

PyObject* Vec3Datum::getCurrentValue()
{
    return areChildrenValid()
        ? PyObject_CallFunctionObjArgs(
            (PyObject*)getType(),
            findChild<Datum*>("x")->getValue(),
            findChild<Datum*>("y")->getValue(),
            findChild<Datum*>("z")->getValue(),
            NULL)
        : NULL;
}
