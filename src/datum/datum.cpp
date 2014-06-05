#include <Python.h>
#include "datum/datum.h"
#include <QDebug>

Datum::Datum(QString name, QObject* parent)
    : QObject(parent), value(NULL), valid(false), input_handler(NULL)
{
    setObjectName(name);
}

Datum::~Datum()
{
    Py_XDECREF(value);
}

void Datum::update()
{
    // Request that all upstream datums disconnect.
    emit disconnectFrom(this);
    PyObject* new_value = getCurrentValue();

    // If our previous value was valid and our new value is invalid,
    // mark valid = false and emit a changed signal.
    if (new_value == NULL && valid)
    {
        valid = false;
        emit changed();
    }
    // If we've gone from invalid to valid or gotten a different object,
    // save the new value and emit changed.
    else if (new_value != NULL && (!valid ||
            PyObject_RichCompareBool(new_value, value, Py_NE)))
    {
        Py_XDECREF(value);
        value = new_value;
        Py_INCREF(value);

        valid = true;

        emit changed();
    }
    Py_XDECREF(new_value);
}

void Datum::onDisconnectRequest(Datum* downstream)
{
    disconnect(downstream, 0, this, 0);
    disconnect(this, 0, downstream, 0);
}

void Datum::connectUpstream(Datum* upstream)
{
    connect(upstream, &Datum::changed,   this, &Datum::update);
    connect(upstream, &Datum::destroyed, this, &Datum::update);
    connect(this, &Datum::disconnectFrom, upstream, &Datum::onDisconnectRequest);
}
