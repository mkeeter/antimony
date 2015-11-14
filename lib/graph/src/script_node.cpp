#include <Python.h>

#include "graph/script_node.h"
#include "graph/graph.h"

ScriptNode::ScriptNode(std::string n, Graph* root)
    : ScriptNode(n, "", root)
{
    // Nothing to do here
}

ScriptNode::ScriptNode(std::string name, uint32_t uid, Graph* root)
    : Node(name, uid, root, false), script(this)
{
    init();
}

ScriptNode::ScriptNode(std::string n, std::string s, Graph* root)
    : Node(n, root, false), script(this)
{
    setScript(s);
    init();
}

ScriptState ScriptNode::getScriptState() const
{
    return script.getState();
}

void ScriptNode::setScript(std::string t)
{
    script.script = t;
    script.trigger();
}

bool ScriptNode::makeDatum(std::string n, PyTypeObject* type,
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
        assert(d->isValid());
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

        // If the datum is an output, update its expression
        if (output)
            d->setText(value);
        // Otherwise, erase the output sigil by setting the text
        else if (d->isOutput())
            d->setText(value);
    }

    script.active.insert(d);

    // Inject this variable into the script's namespace
    script.inject(n.c_str(), d->currentValue());
    saveLookup(n, &script);

    return true;
}

void ScriptNode::loadScriptHooks(PyObject* g)
{
    parent->loadScriptHooks(g, this);
}

void ScriptNode::update(const std::unordered_set<Datum*>& active)
{
    // Remove any datums that weren't marked as active and trigger
    // changes to anything that was watching them.
    std::list<Datum*> inactive;
    for (const auto& d : datums)
        if (active.find(d.get()) == active.end())
            inactive.push_back(d.get());
    for (auto d : inactive)
        uninstall(d);

    triggerWatchers();
}

