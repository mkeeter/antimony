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
    for (NameDatum* d : findChildren<NameDatum*>("__name"))
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
        NameDatum* name = n->getDatum<NameDatum>("__name");

        if (name->getValid())
        {
            auto p = n->proxy(caller);
            PyDict_SetItem(d, name->getValue(), p);
            Py_DECREF(p);
        }
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
