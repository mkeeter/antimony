#include <Python.h>

#include "node/node.h"
#include "node/manager.h"
#include "node/proxy.h"

#include "datum/datum.h"
#include "datum/name.h"

Node::Node(QString name, QObject* parent) :
    QObject(parent), control(NULL)
{
    if (parent == NULL)
    {
        setParent(NodeManager::manager());
    }
    new NameDatum("name", name, this);
}


PyObject* Node::proxy()
{
    PyObject* p = PyObject_CallObject(proxyType(), NULL);
    ((proxy_ProxyObject*)p)->node = this;
    ((proxy_ProxyObject*)p)->caller = NULL;
    return p;
}

Datum* Node::getDatum(QString name)
{
    return findChild<Datum*>(name);
}
