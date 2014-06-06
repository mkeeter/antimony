#include <Python.h>

#include "datum/connection.h"
#include "datum/datum.h"

Connection::Connection(Datum* parent) :
    QObject(parent), control(NULL)
{
    // Nothing to do here
}
