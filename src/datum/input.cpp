#include <Python.h>

#include "datum/input.h"
#include "datum/datum.h"

InputHandler::InputHandler(QObject *parent) :
    QObject(parent)
{
    // Nothing to do here
}

////////////////////////////////////////////////////////////////////////////////

PyObject* SingleInputHandler::getValue() const
{
    if (in)
    {
        return in->getValue();
    }
    return NULL;
}

bool SingleInputHandler::accepts(Datum* input) const
{
    // Need to compare datum type here
}

void SingleInputHandler::addInput(Datum* input)
{
    in = QPointer<Datum>(input);
}
