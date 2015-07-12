#include "graph/datum.h"
#include "graph/node.h"
#include "graph/util.h"
#include "graph/graph.h"
#include "graph/watchers.h"
#include "graph/proxy.h"

const char Datum::SIGIL_CONNECTION = '$';
const char Datum::SIGIL_OUTPUT = '#';

std::unordered_set<char> Datum::sigils = {
    SIGIL_CONNECTION, SIGIL_OUTPUT
};

std::unordered_map<PyTypeObject*, PyObject*> Datum::reducers;

////////////////////////////////////////////////////////////////////////////////

Datum::Datum(std::string name, std::string s,
             PyTypeObject* type, Node* parent)
    : name(name), uid(parent->install(this)), expr(s),
      value(NULL), valid(false), type(type), parent(parent)
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
    return !expr.empty() && (expr.front() != SIGIL_OUTPUT);
}

PyObject* Datum::getValue()
{
    PyObject* locals = parent->parent->proxyDict(NULL, this);
    PyObject* globals = Proxy::getDict(locals);

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
    if (isLink())
        links.insert(d);
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

std::pair<PyObject*, bool> Datum::checkLinkResult(PyObject* obj)
{
    if (error.empty())
    {
        assert(obj != NULL);
        assert(PyList_Check(obj));

        // If the list has been pruned down to emptiness, construct a new
        // expression, assign it, and indicate that we recursed.
        if (PyList_Size(obj) == 0)
        {
            assert(value);
            auto s = PyObject_Str(value);
            assert(!PyErr_Occurred());

            setText(std::string(PyUnicode_AsUTF8(s)));
            Py_DECREF(s);
            return std::pair<PyObject*, bool>(NULL, true);
        }

        // Get the first item from the list, then use the default reducer
        // function to fold all of the other items into it (if possible)
        auto start = PyList_GetItem(obj, 0);
        Py_INCREF(start);

        assert(PyList_Size(obj) == 1 || reducers.count(type) != 0);
        for (int i=1; i < PyList_Size(obj); ++i)
        {
            auto next = PyObject_CallFunctionObjArgs(
                    reducers[type], start, PyList_GetItem(obj, i));
            Py_DECREF(start);
            start = next;
        }
        Py_DECREF(obj);
        return std::make_pair(start, false);
    }
    else if (error.find("is not defined") != std::string::npos)
    {
        assert(obj == NULL);

        // Do a bit of string splicing to cut out the offending UIDs
        size_t index = 2;
        for (size_t i=0; i < links.size(); ++i)
        {
            const size_t found = expr.find(", ", index);
            assert(found != std::string::npos);
            index = found + 2;
        }

        size_t end = expr.find(", ", index);
        end = (end == std::string::npos) ? expr.find("]") : (end + 2);

        // Recurse, then return a tuple indicating that we recursed.
        setText(expr.substr(0, index) + expr.substr(end));
        return std::pair<PyObject*, bool>(NULL, true);
    }
    else
    {
        return std::make_pair(obj, false);
    }
}

void Datum::update()
{
    // Cache the source list to detect if it has changed.
    const auto old_sources = sources;

    // Reset all of the variables that will be populated during evaluation
    sources.clear();
    sources.insert(this);
    links.clear();
    error.clear();

    PyObject* new_value = getValue();

    // Handle link results in a special way, handling disconnection,
    // list pruning, and reducers.
    if (isLink())
    {
        auto r = checkLinkResult(new_value);
        if (r.second)
            return;
        else
            new_value = r.first;
    }

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
    return (DatumState){trimmed.first, !trimmed.second, valid, error};
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

bool Datum::acceptsLink(Datum* upstream) const
{
    // If the types disagree, then we can't make a link
    if (upstream->getType() != type)
        return false;

    // If we don't already have a link, then we can make one
    if (!isLink())
        return true;

    // If we already have a link input and can't reduce, return false
    if (reducers.count(type) == 0)
        return false;

    // Use a regex to find every uid.uid element in the list;
    // if one matches, then return false.
    static std::regex id_regex("__([0-9]+)\\.__([0-9]+)");
    std::smatch match;
    size_t index = 0;
    while (std::regex_search(expr.substr(index), match, id_regex))
    {
        if (std::stoull(match[1]) == upstream->parent->uid &&
            std::stoull(match[2]) == upstream->uid)
            return false;
        index += match[0].length();
    }

    return true;
}

bool Datum::allowLookupByUID() const
{
    return isLink();
}

bool Datum::isLink() const
{
    return !expr.empty() && (expr.front() == SIGIL_CONNECTION);
}

void Datum::installReducer(PyTypeObject* t, PyObject* f)
{
    reducers[t] = f;
}

/*
void Datum::update()
{
    if (input && input->isActive())
        load(input.get());
    else if (expr)
        load(expr.get());
}

void Datum::load(Source* s)
{
    PyObject* out = s->getValue();
    value.load(out, Downstream::sources);
}

bool Datum::canSetText() const
{
    if (input)
        return (!input->isActive() && expr);
    else
        return bool(expr);
}

bool Datum::isTextSimple() const
{
}

void Datum::setText(std::string txt)
{
    assert(canSetText());
    expr->load(txt);
    Downstream::trigger();
}

Datum* Datum::OutputDatum(std::string name, PyObject* obj,
                          Node* parent)
{
    auto d = new Datum(name, obj->ob_type, parent);
    d->value.value = obj;
    d->value.valid = true;
    return d;
}
*/
