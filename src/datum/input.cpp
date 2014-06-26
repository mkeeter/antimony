#include <Python.h>
#include <QDebug>

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
    return in.isNull() &&
            dynamic_cast<Datum*>(parent())->getType() ==
            dynamic_cast<Datum*>(input->parent())->getType();
}

void SingleInputHandler::addInput(Link* input)
{
    Q_ASSERT(in.isNull());
    in = QPointer<Link>(input);
    dynamic_cast<Datum*>(parent())->update();
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

void SingleInputHandler::deleteInput(Datum *d)
{
    Q_ASSERT(!in.isNull() && in->parent() == d);
    delete in;
    in.clear();
}

QString SingleInputHandler::getString() const
{
    Q_ASSERT(!in.isNull());
    return dynamic_cast<Datum*>(in->parent())->getString();
}

////////////////////////////////////////////////////////////////////////////////

ShapeInputHandler::ShapeInputHandler(Datum* parent)
    : InputHandler(parent)
{
    // Nothing to do here
}

PyObject* ShapeInputHandler::getValue() const
{
    /** If there are no legitimate inputs, then return an empty shape.
     */
    if (inputCount() == 0)
    {
        PyObject* globals = Py_BuildValue("{}");
        PyDict_SetItemString(globals, "__builtins__", PyEval_GetBuiltins());
        PyObject* locals = Py_BuildValue("{}");

        PyObject* out = PyRun_String("__import__('fab').Shape('f1.0')",
                                     Py_eval_input, globals, locals);

        Py_DECREF(globals);
        Py_DECREF(locals);

        return out;
    }

    /** Otherwise, OR together all of the input shapes
     *  (or return NULL if any of them are invalid)
     */
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
        Datum* source = dynamic_cast<Datum*>(i->parent());
        Datum* target = dynamic_cast<Datum*>(parent());

        Q_ASSERT(source);
        Q_ASSERT(target);

        valid &= target->connectUpstream(source) && source->getValid();

        if (!valid)
        {
            continue;
        }

        if (out == NULL)
        {
            out = source->getValue();
            Py_INCREF(out);
        }
        else
        {
            PyObject* next = PyObject_CallMethodObjArgs(
                        out, or_function, source->getValue());
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
    return dynamic_cast<Datum*>(parent())->getType() ==
           dynamic_cast<Datum*>(input->parent())->getType();
}

void ShapeInputHandler::addInput(Link* input)
{
    prune();
    in << QPointer<Link>(input);
    dynamic_cast<Datum*>(parent())->update();
}

bool ShapeInputHandler::hasInput() const
{
    return true;
}

void ShapeInputHandler::prune()
{
    QList<QPointer<Link>> new_in;
    for (auto i : in)
    {
        if (!i.isNull())
        {
            new_in << i;
        }
    }
    in = new_in;
}

int ShapeInputHandler::inputCount() const
{
    int count = 0;
    for (auto i : in)
    {
        if (!i.isNull())
        {
            count++;
        }
    }
    return count;
}

void ShapeInputHandler::deleteInput(Datum *d)
{
    bool found = false;
    for (auto i : in)
    {
        if (!i.isNull() && i->parent() == d)
        {
            delete i;
            found = true;
        }
    }
    Q_ASSERT(found);
    prune();
}

QString ShapeInputHandler::getString() const
{
    if (in.length() == 0)
    {
        return QString("No inputs.");
    }
    else if (in.length() == 1)
    {
        return QString("1 input.");
    }
    else
    {
        return QString::number(in.length()) + " inputs";
    }
}
