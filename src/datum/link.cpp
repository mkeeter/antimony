#include <Python.h>

#include <QDebug>

#include "datum/link.h"
#include "datum/datum.h"

Link::Link(Datum* parent) :
    QObject(parent)
{
    // Nothing to do here
}

void Link::setTarget(Datum *d)
{
    target = QPointer<Datum>(d);
}
