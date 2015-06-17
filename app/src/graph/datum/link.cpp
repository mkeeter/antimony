#include <Python.h>

#include <QDebug>

#include "graph/datum/link.h"
#include "graph/datum/datum.h"

Link::Link(Datum* parent) :
    QObject(parent)
{
    // Nothing to do here
}

void Link::setTarget(Datum *d)
{
    target = QPointer<Datum>(d);
}

Datum* Link::getTarget() const
{
    return target;
}
