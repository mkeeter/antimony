#include <Python.h>
#include <QDebug>

#include "graph/datum/input.h"
#include "graph/datum/datum.h"
#include "graph/datum/link.h"

InputHandler::InputHandler(Datum *parent)
    : QObject(parent)
{
    // Nothing to do here
}

QList<Datum*> InputHandler::getInputDatums() const
{
    QList<Datum*> list;
    for (auto link : getLinks())
    {
        Q_ASSERT(dynamic_cast<Datum*>(link->parent()));
        list << static_cast<Datum*>(link->parent());
    }
    return list;
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

    Q_ASSERT(dynamic_cast<Datum*>(in->parent()));
    Datum* source = static_cast<Datum*>(in->parent());

    Q_ASSERT(dynamic_cast<Datum*>(parent()));
    Datum* target = static_cast<Datum*>(parent());

    if (target->connectUpstream(source) && source->getValid())
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
    Q_ASSERT(dynamic_cast<Datum*>(parent()));
    Q_ASSERT(dynamic_cast<Datum*>(input->parent()));

    return in.isNull() &&
            static_cast<Datum*>(parent())->getType() ==
            static_cast<Datum*>(input->parent())->getType();
}

void SingleInputHandler::addInput(Link* input)
{
    Q_ASSERT(in.isNull());
    in = QPointer<Link>(input);
    Q_ASSERT(dynamic_cast<Datum*>(parent()));
    static_cast<Datum*>(parent())->update();
}

bool SingleInputHandler::hasInput() const
{
    if (in.isNull())
    {
        return false;
    }

    // If a datum has been destroyed, it emits the destroyed signal before
    // its children are destroyed.  Because the destroyed signal causes
    // downstream datums to update, hasInput can be called with a un-deleted
    // connection that has a deleted datum as a parent.
    //
    // The strange construct below uses dynamic_cast to see whether this is
    // the case (because if everything but the QObject has been deleted, the
    // dynamic cast will fail).
    Datum* d = dynamic_cast<Datum*>(in->parent());
    return d != NULL;
}

QString SingleInputHandler::getString() const
{
    Q_ASSERT(!in.isNull());
    Q_ASSERT(dynamic_cast<Datum*>(in->parent()));
    return static_cast<Datum*>(in->parent())->getString();
}

QList<Link*> SingleInputHandler::getLinks() const
{
    QList<Link*> links;
    if (in)
        links << in;
    return links;
}

////////////////////////////////////////////////////////////////////////////////

ShapeInputHandler::ShapeInputHandler(Datum* parent)
    : InputHandler(parent)
{
    // Nothing to do here
}

PyObject* ShapeInputHandler::getValue() const
{
    // Otherwise, OR together all of the input shapes
    // (or return NULL if any of them are invalid)
    PyObject* out = NULL;
    bool valid = true;
    PyObject* or_function = PyUnicode_FromString("__or__");
    for (auto i : in)
    {
        // We check to see if the pointer has been deleted or has been reduced
        // to a QObject (because destroyed is emitted before complete destruction,
        // see note above in SingleInputHandler::hasInput)
        if (i.isNull() || dynamic_cast<Datum*>(i->parent()) == NULL)
        {
            continue;
        }

        // Link source and target
        Q_ASSERT(dynamic_cast<Datum*>(i->parent()));
        Datum* source = static_cast<Datum*>(i->parent());
        Q_ASSERT(dynamic_cast<Datum*>(parent()));
        Datum* target = static_cast<Datum*>(parent());

        valid &= target->connectUpstream(source) && source->getValid();

        if (!valid)
            continue;

        if (out == NULL)
        {
            out = source->getValue();
            Py_INCREF(out);
        }
        else
        {
            PyObject* next = PyObject_CallMethodObjArgs(
                        out, or_function, source->getValue(), NULL);
            Py_DECREF(out);
            out = next;

        }
    }

    Py_DECREF(or_function);
    if (valid)
    {
        return out;
    }
    else
    {
        Py_XDECREF(out);
        return NULL;
    }
}

bool ShapeInputHandler::accepts(Link* input) const
{
    Q_ASSERT(dynamic_cast<Datum*>(parent()));
    Q_ASSERT(dynamic_cast<Datum*>(input->parent()));
    return static_cast<Datum*>(parent())->getType() ==
           static_cast<Datum*>(input->parent())->getType();
}

void ShapeInputHandler::addInput(Link* input)
{
    prune();
    in << QPointer<Link>(input);
    Q_ASSERT(dynamic_cast<Datum*>(parent()));
    static_cast<Datum*>(parent())->update();
}

bool ShapeInputHandler::hasInput() const
{
    return inputCount() != 0;
}

void ShapeInputHandler::prune()
{
    QList<QPointer<Link>> new_in;
    for (auto i : in)
        if (!i.isNull())
            new_in << i;
    in = new_in;
}

int ShapeInputHandler::inputCount() const
{
    int count = 0;
    for (auto i : in)
        if (!i.isNull())
            count++;
    return count;
}

QString ShapeInputHandler::getString() const
{
    int count = inputCount();
    if (count == 0)
        return QString("No inputs.");
    else if (count == 1)
        return QString("1 input.");
    else
        return QString::number(in.length()) + " inputs";
}

QList<Link*> ShapeInputHandler::getLinks() const
{
    QList<Link*> links;
    for (auto link : in)
        if (link)
            links << link;
    return links;
}
