#include <Python.h>

#include "datum/link.h"
#include "datum/datum.h"

Link::Link(Datum* parent) :
    QObject(parent), control(NULL)
{
    // Nothing to do here
}
