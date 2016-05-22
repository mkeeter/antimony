#include <Python.h>

#include "graph/node.h"
#include "graph/graph_node.h"
#include "graph/graph.h"
#include "graph/datum.h"
#include "graph/proxy.h"
#include "graph/watchers.h"

Node::Node(std::string n, Graph* root, bool do_init)
    : name(n.back() == '*' ? root->nextName(n.substr(0, n.size() - 1)) : n),
      uid(root->install(this)), parent(root)
{
    if (do_init)
        init();
}

Node::Node(std::string n, uint64_t uid, Graph* root, bool do_init)
    : name(n.back() == '*' ? root->nextName(n.substr(0, n.size() - 1)) : n),
      uid(uid), parent(root)
{
    root->install(this);
    if (do_init)
        init();
}

void Node::init()
{
    parent->changed(name, uid);
    parent->triggerWatchers();
}

NodeState Node::getState() const
{
    return (NodeState){
        isNameValid(name) && parent->isNameUnique(name, this),
        childDatums()};
}

std::string Node::getFullName() const
{
    auto out = name;
    if (auto n = parent->parentNode())
    {
        out = n->getFullName() + "." + out;
    }
    return out;
}

void Node::setName(std::string new_name)
{
    if (new_name != name)
    {
        const std::string old_name = name;
        name = new_name;
        parent->changed(old_name, uid);
        parent->changed(new_name, uid);

        triggerWatchers();
    }
}

std::list<Datum*> Node::childDatums() const
{
    std::list<Datum*> out;
    for (const auto& ptr : datums)
        out.push_back(ptr.get());
    return out;
}

void Node::uninstall(Datum* d)
{
    auto out = d->outgoingLinks();
    for (auto o : out)
        o->uninstallLink(d);
    Root::uninstall(d, &datums);
}

PyObject* Node::mutableProxy()
{
    return Proxy::makeProxyFor(this, NULL, Proxy::FLAG_MUTABLE);
}

Datum* Node::getDatum(std::string name) const
{
    return get(name, datums);
}

void Node::loadDatumHooks(PyObject* g)
{
    parent->loadDatumHooks(g);
}

void Node::pySetAttr(std::string name, PyObject* obj, uint8_t flags)
{
    (void)flags;
    assert(flags & Proxy::FLAG_MUTABLE);

    auto d = getByName(name, datums);
    if (!d)
        throw Proxy::Exception("No datum with name '" + name + "' found.");

    if (d->type != obj->ob_type)
    {
        auto cast = PyObject_CallFunctionObjArgs(
                (PyObject*)d->type, obj, NULL);

        if (PyErr_Occurred())
        {
            PyErr_Clear();
            throw Proxy::Exception("Assignment failed due to invalid type");
        }
        Py_DECREF(obj);
        obj = cast;
    }

    // If the datum is can be directly converted into the given type,
    // then make the assignment; otherwise, skip assignment.  This is
    // so that drag functions behave as intended: dragging "12.0" will
    // change its value, but dragging "n.x" will not.
    auto o = PyObject_CallFunction((PyObject*)d->type, "s", d->expr.c_str());
    if (!o)
    {
        PyErr_Clear();
    }
    else
    {
        auto obj_str = PyObject_Repr(obj);
        d->setText(std::string(PyUnicode_AsUTF8(obj_str)));
        Py_DECREF(obj_str);
    }
}

void Node::queue(Downstream* d)
{
    parent->queue(d);
}

void Node::flushQueue()
{
    parent->flushQueue();
}

PyObject* Node::pyGetAttr(std::string n, Downstream* caller,
                          uint8_t flags) const
{
    auto d = (flags & Proxy::FLAG_UID_LOOKUP)
        ? get(n, datums) : getByName(n, datums);

    if (!d)
        return NULL;

    // If the caller is a datum as well, check for recursive lookups.
    auto datum = dynamic_cast<Datum*>(caller);
    if (datum && !datum->addUpstream(d))
        throw Proxy::Exception("Recursive lookup of datum '" + n + "'");

    if (!d->valid)
        throw Proxy::Exception("Datum '" + n + "' is invalid");

    Py_INCREF(d->value);
    return d->value;
}
