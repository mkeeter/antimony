#include "graph/datum.h"
#include "graph/node.h"
#include "graph/graph_node.h"
#include "graph/util.h"
#include "graph/graph.h"
#include "graph/watchers.h"
#include "graph/proxy.h"

const char Datum::SIGIL_NONE = 0;
const char Datum::SIGIL_CONNECTION = 0x11;
const char Datum::SIGIL_OUTPUT = 0x12;
const char Datum::SIGIL_SUBGRAPH_CONNECTION = 0x13;
const char Datum::SIGIL_SUBGRAPH_OUTPUT = 0x14;

std::unordered_set<char> Datum::sigils = {
    SIGIL_CONNECTION, SIGIL_OUTPUT,
    SIGIL_SUBGRAPH_CONNECTION, SIGIL_SUBGRAPH_OUTPUT,
};

std::unordered_map<PyTypeObject*, PyObject*> Datum::reducers;

////////////////////////////////////////////////////////////////////////////////

Datum::Datum(std::string name, std::string s,
             PyTypeObject* type, Node* parent)
    : name(name), uid(parent->install(this)), expr(s), value(NULL), valid(false),
      type(type), parent(parent)
{
    init();
}

Datum::Datum(std::string name, uint64_t uid, std::string expr,
             PyTypeObject* type, Node* parent)
    : name(name), uid(uid), expr(expr), value(NULL), valid(false),
      type(type), parent(parent)
{
    parent->install(this);
    init();
}

void Datum::init()
{
    // Special-case if the datum is constructed with an empty list of
    // connections as its expression (often due to copy-pasting)
    if (expr.size() == 3 && (expr[0] == SIGIL_CONNECTION ||
                             expr[0] == SIGIL_SUBGRAPH_CONNECTION))
    {
        expr = "";
    }

    // Attempt to update our value
    trigger();

    // Attempt to default-construct an object of the given type
    if (!valid)
    {
        value = PyObject_CallFunctionObjArgs((PyObject*)type, NULL);
        if (PyErr_Occurred())
            PyErr_Clear();
        else
            parent->changed(name, uid);
    }
}

Datum::~Datum()
{
    Py_XDECREF(value);
}

PyObject* Datum::getValue()
{
    Graph* const env = environment();

    PyObject* locals = Proxy::makeProxyFor(env,
            this, isLink() ? Proxy::FLAG_UID_LOOKUP : 0);

    PyObject* globals = Py_BuildValue(
            "{sO}", "__builtins__", PyEval_GetBuiltins());
    env->loadDatumHooks(globals);
    Proxy::setGlobals(locals, globals);

    // If the string begins with a sigil, slice it off
    const std::string e = trimSigil(expr).first;
    PyObject* out = PyRun_String(e.c_str(), Py_eval_input, globals, locals);

    if (PyErr_Occurred())
    {
        error = getPyError().first;
        PyErr_Clear();
    }

    Py_DECREF(locals);
    Py_DECREF(globals);

    return out;
}

bool Datum::addUpstream(Datum* d)
{
    sources.insert(d->sources.begin(), d->sources.end());
    return d->sources.find(this) == d->sources.end();
}

PyObject* Datum::castToType(PyObject* value)
{
    auto cast = PyObject_CallFunctionObjArgs((PyObject*)type, value, NULL);
    if (PyErr_Occurred())
    {
        PyErr_Clear();
        auto actual_type = PyObject_Str((PyObject*)value->ob_type);
        auto desired_type = PyObject_Str((PyObject*)type);
        error = "Could not convert from " +
                std::string(PyUnicode_AsUTF8(actual_type)) +
                " to " + std::string(PyUnicode_AsUTF8(desired_type));
        Py_DECREF(actual_type);
        Py_DECREF(desired_type);
    }
    Py_DECREF(value);
    return cast;
}

std::unordered_set<const Datum*> Datum::getLinks() const
{
    std::unordered_set<const Datum*> out;
    if (!isLink())
        return out;

    // FIXME: #accidentallyquadratic
    std::string links = expr.substr(2);  // skip initial SIGIL + "["
    while (1)
    {
        // Get our execution environment, which is either the parent node's
        // parent graph (in normal cases) or the parent node itself (when the
        // datum belongs to a subgraph)
        auto graph = environment();

        // Get our parent node, which is a node in the graph
        // that this datum belongs to.
        Node* node = links.find("__parent") == 0
                ? graph->parentNode()
                : graph->getNode(stoull(links.substr(2)));
        links = links.substr(links.find(".") + 1);

        // Extract datum UID
        if (node)
        {
            const uint64_t datum_uid = std::stoull(links.substr(2));
            if (auto datum = node->getDatum(datum_uid))
                out.insert(datum);
        }

        const size_t next = links.find(",");
        if (next == std::string::npos)
            break;
        else
            links = links.substr(next + 1);
    }

    return out;
}

std::unordered_set<Datum*> Datum::outgoingLinks() const
{
    std::unordered_set<Datum*> out;
    auto range = parent->lookups.equal_range("__" + std::to_string(uid));
    for (auto it = range.first; it != range.second; ++it)
    {
        assert(dynamic_cast<Datum*>(it->second));
        out.insert(static_cast<Datum*>(it->second));
    }
    return out;
}

void Datum::writeLinkExpression(const std::unordered_set<const Datum*> links)
{
    // If the list has been pruned down to emptiness, construct a new
    // expression by calling 'str' on the existing value.
    std::string out;
    if (links.empty())
    {
        assert(value);
        auto s = PyObject_Repr(value);
        assert(!PyErr_Occurred());

        // Get string from Python
        const std::string e(PyUnicode_AsUTF8(s));

        // Preserve subgraph sigil if present
        out = isFromSubgraph() ? SIGIL_SUBGRAPH_OUTPUT + e : e;
    }
    else
    {
        out = (isFromSubgraph() ? SIGIL_SUBGRAPH_CONNECTION
                                : SIGIL_CONNECTION) + std::string("[");
        for (auto d : links)
        {
            if (out.back() != '[')
                out += ",";
            out += formatLink(d);
        }
        out += "]";
    }
    setText(out);
}

std::string Datum::formatLink(const Datum* upstream) const
{
    std::string id;

    Graph* const env = environment();

    // First, check to see if the upstream comes from the
    // same graph level as this datum
    if (upstream->parent->parent == env)
        id = "__" + std::to_string(upstream->parent->uid);

    // Then, check to see if the upstream comes from the
    // supergraph that contains this datum's parent node.
    else if (upstream->parent == env->parentNode())
        id = "__parent";

    // Otherwise, we don't know what to do: datum links are only
    // allowed to happen across one level of a graph.
    else
        assert(false);

    id += ".__" + std::to_string(upstream->uid);

    return id;
}

void Datum::installLink(const Datum* upstream)
{
    assert(acceptsLink(upstream));

    std::string id = formatLink(upstream);

    if (isLink())
        setText(expr.substr(0, expr.size() - 1) + "," + id + "]");
    else
        setText((isFromSubgraph() ? SIGIL_SUBGRAPH_CONNECTION
                                  : SIGIL_CONNECTION) + ("[" + id + "]"));
}

void Datum::uninstallLink(const Datum* upstream)
{
    auto links = getLinks();
    assert(getLinks().count(upstream) == 1);

    links.erase(upstream);
    writeLinkExpression(links);
    trigger();
}

PyObject* Datum::checkLinkResult(PyObject* obj)
{
    if (error.empty())
    {
        assert(obj != NULL);
        assert(PyList_Check(obj));

        // Get the first item from the list, then use the default reducer
        // function to fold all of the other items into it (if possible)
        auto start = PyList_GetItem(obj, 0);
        Py_INCREF(start);

        assert(PyList_Size(obj) == 1 || reducers.count(type) != 0);
        for (int i=1; i < PyList_Size(obj); ++i)
        {
            auto next = PyObject_CallFunctionObjArgs(
                    reducers[type], start, PyList_GetItem(obj, i), NULL);
            Py_DECREF(start);
            start = next;
        }
        Py_DECREF(obj);
        return start;
    }
    else
    {
        return obj;
    }
}

void Datum::update()
{
    // Cache the source list to detect if it has changed.
    const auto old_sources = sources;

    // Reset all of the variables that will be populated during evaluation
    sources.clear();
    sources.insert(this);
    error.clear();

    PyObject* new_value = getValue();

    // Handle link results in a special way (with indexing and reducing)
    if (isLink())
        new_value = checkLinkResult(new_value);

    // Check output types and cast if necessary
    if (new_value && new_value->ob_type != type)
        new_value = castToType(new_value);

    // If our previous value was valid and our new value is invalid,
    // mark valid = false and emit a changed signal.
    bool changed = false;
    if (new_value == NULL && valid)
    {
        valid = false;
        changed = true;
    }

    // If we've gone from invalid to valid or gotten a different object,
    // save the new value and emit changed.
    else if (new_value != NULL && (!valid ||
            PyObject_RichCompareBool(new_value, value, Py_NE)))
    {
        Py_XDECREF(value);
        value = new_value;
        Py_INCREF(value);

        valid = true;
        changed = true;
    }
    Py_XDECREF(new_value);

    if (sources != old_sources)
        changed = true;

    if (changed)
        parent->changed(name, uid);

    if (!watchers.empty())
    {
        const auto state = getState();
        for (auto w : watchers)
            w->trigger(state);
    }

    const auto new_links = getLinks();
    for (auto o : links)
    {
        if (!new_links.count(o))
        {
            o->triggerWatchers();
        }
    }
    for (auto n : new_links)
    {
        if (!links.count(n))
        {
            n->triggerWatchers();
        }
    }
}

DatumState Datum::getState() const
{
    auto trimmed = trimSigil(expr);

    auto r = PyObject_Repr(value);
    std::string repr(PyUnicode_AsUTF8(r));
    Py_DECREF(r);

    return (DatumState){
        trimmed.first, repr, trimmed.second ? expr.front() : SIGIL_NONE,
        valid, error, getLinks()};
}

std::pair<std::string, bool> Datum::trimSigil(std::string e)
{
    const bool has_sigil = !e.empty() &&
                           (sigils.find(e.front()) != sigils.end());
    return std::make_pair(has_sigil ? e.substr(1) : e, has_sigil);
}

void Datum::setText(std::string s)
{
    if (s != expr)
    {
        links = getLinks();
        expr = s;
        trigger();
    }
}

bool Datum::acceptsLink(const Datum* upstream) const
{
    // If the types disagree, then we can't make a link
    if (upstream->getType() != type)
        return false;

    // If adding this link would create a recursive loop, reject it
    if (upstream->sources.count(this) != 0)
        return false;

    // If we don't already have a link, then we can make one
    if (!isLink())
        return true;

    // If we already have a link input and can't reduce, return false
    if (reducers.count(type) == 0)
        return false;

    // Otherwise, return true if we don't already a link to this datum.
    auto links = getLinks();
    return links.count(upstream) == 0;
}

bool Datum::isLink() const
{
    return !expr.empty() && (expr.front() == SIGIL_CONNECTION ||
                             expr.front() == SIGIL_SUBGRAPH_CONNECTION);
}

bool Datum::isFromSubgraph() const
{
    return !expr.empty() && (expr.front() == SIGIL_SUBGRAPH_CONNECTION ||
                             expr.front() == SIGIL_SUBGRAPH_OUTPUT);
}

bool Datum::isOutput() const
{
    return !expr.empty() && (expr.front() == SIGIL_OUTPUT ||
                             isFromSubgraph());
}

Graph* Datum::environment() const
{
    return isFromSubgraph() ? static_cast<GraphNode*>(parent)->getGraph()
                            : parent->parent;
}

void Datum::installReducer(PyTypeObject* t, PyObject* f)
{
    reducers[t] = f;
}
