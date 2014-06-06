#include <Python.h>

#include <QDebug>

#include "datum/datum.h"
#include "datum/input.h"
#include "datum/link.h"

Datum::Datum(QString name, QObject* parent)
    : QObject(parent), value(NULL), valid(false), input_handler(NULL)
{
    setObjectName(name);
}

Datum::~Datum()
{
    Py_XDECREF(value);
}

bool Datum::hasInputValue() const
{
    return input_handler != NULL && input_handler->hasInput();
}

bool Datum::canEdit() const
{
    return !hasInputValue();
}

bool Datum::acceptsLink(Link* upstream) const
{
    return input_handler->accepts(upstream);
}

Link* Datum::linkFrom()
{
    return new Link(this);
}

void Datum::addLink(Link* input)
{
    input_handler->addInput(input);
}

void Datum::update()
{
    // Request that all upstream datums disconnect.
    emit disconnectFrom(this);

    PyObject* new_value;
    if (hasInputValue())
    {
        new_value = input_handler->getValue();
    }
    else
    {
        new_value = getCurrentValue();
    }

    bool has_changed = false;
    // If our previous value was valid and our new value is invalid,
    // mark valid = false and emit a changed signal.
    if (new_value == NULL && valid)
    {
        valid = false;
        has_changed = true;
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

        has_changed = true;
    }
    Py_XDECREF(new_value);

    // If our editable state has changed, mark that we need to emit the
    // changed signal (so that node viewers can modify their lineedits)
    if (canEdit() != editable)
    {
        editable = canEdit();
        has_changed = true;
    }

    if (has_changed)
    {
        emit changed();
    }
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
