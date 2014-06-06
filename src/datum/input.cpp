#include <Python.h>

#include "datum/input.h"
#include "datum/datum.h"
#include "datum/link.h"

InputHandler::InputHandler(Datum *parent)
    : QObject(parent)
{
    // Nothing to do here
}

////////////////////////////////////////////////////////////////////////////////

SingleInputHandler::SingleInputHandler(Datum *parent)
    : InputHandler(parent)
{
    // Nothing to do here
}

PyObject* SingleInputHandler::getValue() const
{
    Q_ASSERT(!in.isNull());

    Datum* source = dynamic_cast<Datum*>(in->parent());
    Datum* target = dynamic_cast<Datum*>(parent());

    Q_ASSERT(source);
    Q_ASSERT(target);

    target->connectUpstream(source);

    if (source->getValid())
    {
        PyObject* v = source->getValue();
        Py_INCREF(v);
        return v;
    }
    else
    {
        return NULL;
    }
}

bool SingleInputHandler::accepts(Link* input) const
{
    return in.isNull() &&
            dynamic_cast<Datum*>(parent())->getType() ==
            dynamic_cast<Datum*>(input->parent())->getType();
}

void SingleInputHandler::addInput(Link* input)
{
    Q_ASSERT(in.isNull());
    in = QPointer<Link>(input);
}

bool SingleInputHandler::hasInput() const
{
    return !in.isNull();
}
