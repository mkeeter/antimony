#include <boost/python.hpp>

#include <QApplication>
#include <QDebug>

#include "graph/node/root.h"
#include "graph/node/node.h"
#include "graph/node/proxy.h"

#include "graph/datum/datum.h"
#include "graph/datum/datums/name_datum.h"

#include "fab/types/shape.h"
#include "fab/fab.h"

NodeRoot::NodeRoot(QObject *parent) :
    QObject(parent)
{
    // Nothing to do here
}

bool NodeRoot::isNameUnique(QString name) const
{
    PyObject* n = PyUnicode_FromString(name.toStdString().c_str());
    bool result = (findMatchingName(n) == NULL);
    Py_DECREF(n);
    return result;
}

NameDatum* NodeRoot::findMatchingName(PyObject* proposed) const
{
    for (NameDatum* d : findChildren<NameDatum*>("_name"))
        if (d->getValid() && PyObject_RichCompareBool(d->getValue(),
                                                      proposed, Py_EQ))
            return d;
    return NULL;
}


QString NodeRoot::getName(QString prefix) const
{
    QString name;
    int i = 0;
    while (true)
    {
        name = prefix + QString::number(i++);
        if (isNameUnique(name))
            break;
    }
    return name;
}

PyObject* NodeRoot::proxyDict(Datum* caller)
{
    PyObject* d = PyDict_New();
    PyDict_SetItemString(d, "__builtins__", PyEval_GetBuiltins());
    PyDict_SetItemString(d, "math", PyImport_ImportModule("math"));
    for (Node* n : findChildren<Node*>())
    {
        NameDatum* name = n->getDatum<NameDatum>("_name");

        if (name->getValid())
            PyDict_SetItem(d, name->getValue(), n->proxy(caller));
    }
    return d;
}

void NodeRoot::onNameChange(QString new_name)
{
    // When a node's name changes, call update on any EvalDatums that
    // contain the new name as a substring.
    for (EvalDatum* e : findChildren<EvalDatum*>())
        if (e->getExpr().indexOf(new_name) != -1)
            e->update();
}

Shape NodeRoot::getCombinedShape()
{
    PyObject* out = NULL;
    PyObject* or_function = PyUnicode_FromString("__or__");

    for (Datum* d : findChildren<Datum*>())
    {
        if (d->getType() != fab::ShapeType ||
            !d->hasOutput() || d->hasConnectedLink())
        {
            continue;
        }

        if (out == NULL)
        {
            out = d->getValue();
            Py_INCREF(out);
        }
        else
        {
            PyObject* next = PyObject_CallMethodObjArgs(
                    out, or_function, d->getValue(), NULL);
            Py_DECREF(out);
            out = next;
        }
    }
    Py_DECREF(or_function);

    if (out == NULL)
        return Shape();
    boost::python::extract<Shape> get_shape(out);

    Q_ASSERT(get_shape.check());
    Shape s = get_shape();
    Py_DECREF(out);

    return s;
}

QMap<QString, Shape> NodeRoot::getShapes()
{
    QMap<QString, Shape> out;
    for (Datum* d : findChildren<Datum*>())
    {
        if (d->getType() != fab::ShapeType ||
                !d->hasOutput() || d->hasConnectedLink())
            continue;

        boost::python::extract<Shape> get_shape(d->getValue());
        Q_ASSERT(get_shape.check());

        Q_ASSERT(dynamic_cast<Node*>(d->parent()));
        Datum* name = static_cast<Node*>(d->parent())->getDatum("_name");
        wchar_t* w = PyUnicode_AsWideCharString(name->getValue(), 0);
        Q_ASSERT(w);
        out[QString::fromWCharArray(w)] = get_shape();
        PyMem_Free(w);
    }
    return out;
}
