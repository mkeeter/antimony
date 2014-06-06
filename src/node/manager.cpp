#include <Python.h>

#include <QApplication>

#include "node/manager.h"
#include "node/node.h"
#include "node/proxy.h"

#include "datum/datum.h"
#include "datum/name.h"

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
    for (NameDatum* d : findChildren<NameDatum*>("name"))
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
    for (Node* n : findChildren<Node*>())
    {
        NameDatum* name = dynamic_cast<NameDatum*>(n->getDatum("name"));
        if (name->getValid())
        {
            PyObject* proxy = n->proxy();
            ((proxy_ProxyObject*)proxy)->caller = caller;
            PyDict_SetItem(d, name->getValue(), proxy);
        }
    }
    return d;
}

NodeManager* NodeManager::manager()
{
    if (_manager == NULL)
    {
        _manager = new NodeManager(QApplication::instance());
    }
    return _manager;
}
