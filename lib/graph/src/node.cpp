#include <Python.h>

#include "graph/node.h"
#include "graph/graph.h"
#include "graph/datum.h"
#include "graph/proxy.h"

Node::Node(std::string n, Graph* root)
    : name(n), parent(root)
{
    parent->install(this);
}

void Node::install(Datum* d)
{
    datums.push_back(d);
}

PyObject* Node::proxyDict(Downstream* caller)
{
    return parent->proxyDict(this, caller);
}

void Node::makeInput(std::string n, PyTypeObject* type, std::string value)
{
    /*
    if (!isNameValid(name))
        throw "Name is not valid!";
        //throw IOHooks::Exception("Name is not valid.");
        */

    auto d = getDatum(n);
    // If the datum is of the wrong type, delete it.
    if (d != NULL && (d->type != type))
    {
        delete d;
        datums.remove(d);
        d = NULL;
    }

    if (d == NULL)
        d = new Datum(n, value, type, this);

    script.active.insert(d);
}

/*
void Node::makeOutput(std::string n, PyObject* out)
{
    if (!Name::check(n, this))
        return;

    // If the datum is missing, of the wrong type, or is an input datum
    // (with input and expr), delete it and start over.
    auto d = getDatum(n);
    if (d != NULL && (d->value.type != out->ob_type ||
                      d->expr || d->input))
    {
        delete d;
        datums.remove(d);
        d = NULL;
    }

    if (d == NULL)
        d = Datum::OutputDatum(n, out, this);

    script.active.insert(d);
}
*/

Datum* Node::getDatum(std::string name) const
{
    auto match = std::find_if(datums.begin(), datums.end(),
                              [&](Datum* d){ return d->name == name; });
    if ((match != datums.end()) && (*match)->valid)
        return *match;
    else
        return NULL;
}

PyObject* Node::pyGetAttr(std::string name, Downstream* caller) const
{
    auto d = getDatum(name);
    if (d && d->valid)
    {
        Py_INCREF(d->value);
        return d->value;
    }
    else
    {
        throw Proxy::Exception("Datum '" + name + "' is invalid");
        return NULL;
    }
}
