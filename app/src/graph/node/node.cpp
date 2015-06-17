#include <boost/python.hpp>

#include <QStringList>
#include <QRegularExpression>

#include "graph/node/node.h"
#include "graph/node/root.h"
#include "graph/node/proxy.h"

#include "graph/datum/datum.h"
#include "graph/datum/link.h"
#include "graph/datum/datums/name_datum.h"
#include "graph/datum/datums/script_datum.h"

Node::Node(NodeRoot* parent)
    : QObject(parent)
{
    // Nothing to do here
}

Node::Node(QString name, NodeRoot* parent)
    : Node(parent)
{
    new NameDatum("__name", name, this);
}

void Node::setParent(NodeRoot* root)
{
    QObject::setParent(root);
    for (auto d : findChildren<NameDatum*>(QString(),
                                           Qt::FindDirectChildrenOnly))
        d->update();
}

void Node::setTitle(QString new_title)
{
    if (new_title != title)
    {
        title = new_title;
        emit(titleChanged(title));
    }
}

void Node::updateName()
{
    Q_ASSERT(dynamic_cast<NodeRoot*>(parent()));
    auto r = static_cast<NodeRoot*>(parent());
    QRegularExpression regex("(.*)[0-9]+");

    for (auto d : findChildren<NameDatum*>(QString(),
                                           Qt::FindDirectChildrenOnly))
    {
        auto match = regex.match(d->getExpr());
        if (match.hasMatch())
            d->setExpr(r->getName(match.captured(1)));
        else
            d->setExpr(r->getName(d->getExpr() + "_"));
    }
}

PyObject* Node::proxy(Datum* caller, bool settable)
{
    auto p = PyObject_CallObject(proxy::proxyConstructor(), NULL);
    Q_ASSERT(!PyErr_Occurred());

    auto& proxy = boost::python::extract<NodeProxy&>(p)();
    proxy.node = this;
    proxy.caller = caller;
    proxy.settable = settable;

    return p;
}

PyObject* Node::mutableProxy()
{
    return proxy(NULL, true);
}

Datum* Node::getDatum(QString name) const
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

QString Node::getName() const
{
    if (auto e = getDatum<EvalDatum>("__name"))
        return e->getExpr();
    return "";
}

QSet<Link*> Node::getLinks() const
{
    QSet<Link*> links;

    for (auto d : findChildren<Datum*>(QString(), Qt::FindDirectChildrenOnly))
    {
        for (auto k : d->findChildren<Link*>())
            links.insert(k);
        for (auto k : d->inputLinks())
            links.insert(k);
    }
    return links;
}

Node* ScriptNode(QString name, QString script, NodeRoot* parent)
{
    Node* n = new Node(name, parent);
    new ScriptDatum("__script", script, n);
    return n;
}

Node* ScriptNode(QString script, NodeRoot* parent)
{
    Node* n = new Node(parent->getName("a"), parent);
    new ScriptDatum("__script", script, n);
    return n;
}

Node* ScriptNode(NodeRoot* parent)
{
    Node* n = new Node(parent->getName("a"), parent);
    new ScriptDatum("__script",
             "from fab import shapes\n\n"
             "title('example')\n"
             "output('c', shapes.circle(0, 0, 1))", n);
    return n;
}

