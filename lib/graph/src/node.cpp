#include <Python.h>

#include "graph/node.h"
#include "graph/graph.h"
#include "graph/datum.h"
#include "graph/proxy.h"

Node::Node(std::string n, Graph* root)
    : name(n), uid(root->install(this)), script(this), parent(root)
{
    // Nothing to do here
}

void Node::setScript(std::string t)
{
    script.script = t;
    script.trigger();
}

void Node::update(const std::unordered_set<Datum*>& active)
{
    datums.remove_if([&](const std::unique_ptr<Datum>& d_)
                     { return active.find(d_.get()) == active.end(); });
}

uint32_t Node::install(Datum* d)
{
    return Root::install<Datum>(d, &datums);
}

PyObject* Node::proxyDict(Downstream* caller)
{
    return parent->proxyDict(this, caller);
}

Datum* Node::getDatum(std::string name) const
{
    return get(name, datums);
}

void Node::makeDatum(std::string n, PyTypeObject* type, std::string value)
{
    // If there's an existing datum and it's of the wrong type, delete it.
    auto d = getDatum(n);
    if (d != NULL && (d->type != type))
    {
        datums.remove_if([&](const std::unique_ptr<Datum>& d_)
                         { return d_.get() == d; });
        d = NULL;
    }

    if (d == NULL)
        d = new Datum(n, value, type, this);
    else
        // Move the existing datum to the end of the list
        // (so that ordering matches ordering in the script)
        for (auto itr = datums.begin(); itr != datums.end(); ++itr)
            if (itr->get() == d)
            {
                datums.splice(datums.end(), datums, itr);
                break;
            }

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

PyObject* Node::pyGetAttr(std::string n, Downstream* caller) const
{
    auto d = get(n, datums);
    if (d)
    {
        // If the caller is a datum as well, check for recursive lookups.
        if (auto datum = dynamic_cast<Datum*>(caller))
        {
            datum->sources.insert(d->sources.begin(), d->sources.end());
            if (d->sources.find(datum) != d->sources.end())
            {
                throw Proxy::Exception("Recursive lookup of datum '" + n + "'");
            }
        }

        if (d->valid)
        {
            Py_INCREF(d->value);
            return d->value;
        }
        throw Proxy::Exception("Datum '" + n + "' is invalid");
    }
    else
    {
        return NULL;
    }
}
