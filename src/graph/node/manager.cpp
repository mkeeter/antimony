#include <boost/python.hpp>

#include <QApplication>
#include <QDebug>

#include "graph/node/manager.h"
#include "graph/node/node.h"
#include "graph/node/proxy.h"

#include "graph/datum/datum.h"
#include "graph/datum/datums/name_datum.h"

#include "fab/types/shape.h"
#include "fab/fab.h"

NodeManager* NodeManager::_manager = NULL;

NodeManager::NodeManager(QObject *parent) :
    QObject(parent)
{
    // Nothing to do here
}

bool NodeManager::isNameUnique(QString name) const
{
    PyObject* n = PyUnicode_FromString(name.toStdString().c_str());
    bool result = (findMatchingName(n) == NULL);
    Py_DECREF(n);
    return result;
}

NameDatum* NodeManager::findMatchingName(PyObject* proposed) const
{
    for (NameDatum* d : findChildren<NameDatum*>("_name"))
    {
        if (d->getValid() &&
            PyObject_RichCompareBool(d->getValue(), proposed, Py_EQ))
        {
            return d;
        }
    }
    return NULL;
}


QString NodeManager::getName(QString prefix) const
{
    QString name;
    int i = 0;
    while (true)
    {
        name = prefix + QString::number(i++);
        if (isNameUnique(name))
        {
            break;
        }
    }
    return name;
}

PyObject* NodeManager::proxyDict(Datum* caller)
{
    PyObject* d = PyDict_New();
    PyDict_SetItemString(d, "__builtins__", PyEval_GetBuiltins());
    PyDict_SetItemString(d, "math", PyImport_ImportModule("math"));
    for (Node* n : findChildren<Node*>())
    {
        NameDatum* name = n->getDatum<NameDatum>("_name");

        if (name->getValid())
        {
            PyObject* proxy = n->proxy();
            ((NodeProxyObject*)proxy)->caller = caller;
            PyDict_SetItem(d, name->getValue(), proxy);
        }
    }
    return d;
}

void NodeManager::clear()
{
    for (auto n : findChildren<Node*>(QString(), Qt::FindDirectChildrenOnly))
    {
        delete n;
    }
}

NodeManager* NodeManager::manager()
{
    if (_manager == NULL)
    {
        _manager = new NodeManager(QApplication::instance());
    }
    return _manager;
}

void NodeManager::onNameChange(QString new_name)
{
    // When a node's name changes, call update on any EvalDatums that
    // contain the new name as a substring.
    for (EvalDatum* e : findChildren<EvalDatum*>())
    {
        if (e->getExpr().indexOf(new_name) != -1)
        {
            e->update();
        }
    }

}

Shape NodeManager::getCombinedShape()
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

    if (out == NULL)
    {
        return Shape();
    }
    boost::python::extract<Shape> get_shape(out);

    Q_ASSERT(get_shape.check());
    Shape s = get_shape();
    Py_DECREF(out);

    return s;
}

QMap<QString, Shape> NodeManager::getShapes()
{
    QMap<QString, Shape> out;
    for (Datum* d : findChildren<Datum*>())
    {
        if (d->getType() != fab::ShapeType ||
            !d->hasOutput() || d->hasConnectedLink())
        {
            continue;
        }
        boost::python::extract<Shape> get_shape(d->getValue());

        Q_ASSERT(get_shape.check());

        Datum* name = dynamic_cast<Node*>(d->parent())->getDatum("_name");
        wchar_t* w = PyUnicode_AsWideCharString(name->getValue(), 0);
        Q_ASSERT(w);
        out[QString::fromWCharArray(w)] = get_shape();
        PyMem_Free(w);
    }
    return out;
}

#ifdef ANTIMONY
#if 0
#include "control/control.h"
#include "ui/connection.h"

void NodeManager::makeControls(Canvas* canvas)
{
    for (auto n : findChildren<Node*>(QString(), Qt::FindDirectChildrenOnly))
    {
        Control::makeControlFor(canvas, n);
    }
}

void NodeManager::makeConnections(Canvas* canvas)
{
    for (auto link : findChildren<Link*>())
    {
        new Connection(link, canvas);
    }
}
#endif

#endif
