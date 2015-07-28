#include "graph/datum.h"
#include "graph/node.h"
#include "graph/util.h"
#include "graph/graph.h"
#include "graph/watchers.h"
#include "graph/proxy.h"

const char Datum::SIGIL_NONE = 0;
const char Datum::SIGIL_CONNECTION = 0x11;
const char Datum::SIGIL_OUTPUT = 0x12;

std::unordered_set<char> Datum::sigils = {
    SIGIL_CONNECTION, SIGIL_OUTPUT
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

bool Datum::hasInput() const
{
    return !isOutput();
}

PyObject* Datum::getValue()
{
    PyObject* locals = parent->parent->proxyDict(this);
    PyObject* globals = Py_BuildValue(
            "{sO}", "__builtins__", PyEval_GetBuiltins());
    parent->parent->loadDatumHooks(globals);
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

    size_t index = 4; // skip initial SIGIL + "[__"
    while (1)
    {
        const size_t node_uid_start = index;
        const size_t node_uid_end = expr.find('.', index);
        assert(node_uid_end != std::string::npos);
        const uint64_t node_uid = std::stoull(
                expr.substr(index, node_uid_end - node_uid_start));

        const size_t datum_uid_start = node_uid_end + 3; // for ".__"
        const size_t datum_uid_end = expr.find(',', datum_uid_start);
        const uint64_t datum_uid = std::stoull(expr.substr(
                    datum_uid_start, (datum_uid_end == std::string::npos)
                        ? expr.size() - datum_uid_start - 1
                        : datum_uid_end - datum_uid_start));

        auto graph = parent->parent;
        if (auto node = graph->getNode(node_uid))
            if (auto datum = node->getDatum(datum_uid))
                out.insert(datum);

        if (datum_uid_end == std::string::npos)
            break;
        index = datum_uid_end + 3; // for ",__"
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
    if (links.empty())
    {
        assert(value);
        auto s = PyObject_Repr(value);
        assert(!PyErr_Occurred());

        expr = std::string(PyUnicode_AsUTF8(s));
    }
    else
    {
        expr = SIGIL_CONNECTION + std::string("[");
        for (auto d : links)
        {
            if (expr.back() != '[')
                expr += ",";
            expr += "__" + std::to_string(d->parent->uid) +
                   ".__" + std::to_string(d->uid);
        }
        expr += "]";
    }
}

void Datum::installLink(const Datum* upstream)
{
    assert(acceptsLink(upstream));

    std::string id = "__" +  std::to_string(upstream->parent->uid) +
                    ".__" + std::to_string(upstream->uid);
    if (isLink())
        setText(expr.substr(0, expr.size() - 1) + "," + id + "]");
    else
        setText(SIGIL_CONNECTION + ("[" + id + "]"));
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

}

DatumState Datum::getState() const
{
    auto trimmed = trimSigil(expr);

    auto r = PyObject_Repr(value);
    std::string repr(PyUnicode_AsUTF8(r));
    Py_DECREF(r);

    return (DatumState){
        trimmed.first, repr, trimmed.second ? expr.front() : SIGIL_NONE,
        !trimmed.second, valid, error, getLinks()};
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

bool Datum::allowLookupByUID() const
{
    return isLink();
}

bool Datum::isLink() const
{
    return !expr.empty() && (expr.front() == SIGIL_CONNECTION);
}

bool Datum::isOutput() const
{
    return !expr.empty() && (expr.front() == SIGIL_OUTPUT);
}

void Datum::installReducer(PyTypeObject* t, PyObject* f)
{
    reducers[t] = f;
}
