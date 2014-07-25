#include <Python.h>

#include <QStringList>

#include "node/node.h"
#include "node/manager.h"
#include "node/proxy.h"

#include "datum/datum.h"
#include "datum/name_datum.h"

Node::Node(QString name, QObject* parent) :
    QObject(parent), control(NULL)
{
    new NameDatum("name", name, this);

    if (parent == NULL)
    {
        setParent(NodeManager::manager());
        connect(getDatum<NameDatum>("name"),
                SIGNAL(nameChanged(QString)),
                NodeManager::manager(),
                SLOT(onNameChange(QString)));
    }
}


PyObject* Node::proxy()
{
    PyObject* p = PyObject_CallObject(proxyType(), NULL);
    Q_ASSERT(p);
    ((NodeProxyObject*)p)->node = this;
    ((NodeProxyObject*)p)->caller = NULL;
    return p;
}

Datum* Node::getDatum(QString name)
{
    QStringList s = name.split(".");

    if (s.length() == 1)
    {
        return findChild<Datum*>(s.back());
    }
    else
    {
        Node* n = findChild<Node*>(s.front());
        if (n)
        {
            s.pop_front();
            return n->getDatum(s.join("."));
        }
    }
    return NULL;
}
