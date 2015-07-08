#include <Python.h>

#include "graph/node.h"
#include "graph/graph.h"
#include "graph/datum.h"
#include "graph/proxy.h"
#include "graph/watchers.h"

Node::Node(std::string n, Graph* root)
    : name(n), uid(root->install(this)), script(this), parent(root)
{
    // Nothing to do here
}

Node::Node(std::string n, std::string script, Graph* root)
    : name(n), uid(root->install(this)), script(this), parent(root)
{
    setScript(script);
}

Node::~Node()
{
    std::list<std::pair<std::string, uint32_t>> ds;
    for (const auto& d : datums)
        ds.push_back(std::make_pair(d->name, d->uid));
    datums.clear();
    for (auto d : ds)
        changed(d.first, d.second);
}

void Node::setScript(std::string t)
{
    script.script = t;
    script.trigger();
}

NodeState Node::getState() const
{
    return (NodeState){
            script.script, script.error, script.output,
            script.error_lineno, childDatums()};
}

std::list<Datum*> Node::childDatums() const
{
    std::list<Datum*> out;
    for (const auto& ptr : datums)
        out.push_back(ptr.get());
    return out;
}

void Node::update(const std::unordered_set<Datum*>& active)
{
    // Remove any datums that weren't marked as active and trigger
    // changes to anything that was watching them.
    std::list<Datum*> inactive;
    for (const auto& d : datums)
        if (active.find(d.get()) == active.end())
            inactive.push_back(d.get());
    for (auto d : inactive)
        uninstall(d);

    if (!watchers.empty())
    {
        auto state =  getState();
        for (auto w : watchers)
            w->trigger(state);
    }
}

PyObject* Node::proxyDict(Downstream* caller)
{
    return parent->proxyDict(this, caller);
}

Datum* Node::getDatum(std::string name) const
{
    return get(name, datums);
}

bool Node::makeDatum(std::string n, PyTypeObject* type,
                     std::string value, bool output)
{
    for (auto a : script.active)
        if (a->name == n)
            return false;

    // If there's an existing datum and it's of the wrong type, delete it.
    auto d = getDatum(n);
    if (d != NULL && (d->type != type))
    {
        datums.remove_if([&](const std::unique_ptr<Datum>& d_)
                         { return d_.get() == d; });
        d = NULL;
    }

    if (d == NULL)
    {
        d = new Datum(n, value, type, this);
    }
    else
    {
        // Move the existing datum to the end of the list
        // (so that ordering matches ordering in the script)
        for (auto itr = datums.begin(); itr != datums.end(); ++itr)
            if (itr->get() == d)
            {
                datums.splice(datums.end(), datums, itr);
                break;
            }

        // And update its expression
        if (output)
            d->setText(value);
    }

    script.active.insert(d);
    return true;
}

PyObject* Node::pyGetAttr(std::string n, Downstream* caller) const
{
    auto d = (caller && caller->allowLookupByUID())
        ? get(n, datums) : getByName(n, datums);

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
