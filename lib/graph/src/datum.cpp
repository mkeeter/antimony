#include "graph/datum.h"
#include "graph/node.h"
#include "graph/util.h"
#include "graph/graph.h"
#include "graph/watchers.h"
#include "graph/proxy.h"

std::unordered_set<char> Datum::sigils = {
    SIGIL_CONNECTION, SIGIL_OUTPUT
};

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

void Datum::update()
{
    // Cache the source list to detect if it has changed.
    const auto old_sources = sources;
    sources.clear();
    sources.insert(this);

    PyObject* new_value = getValue();

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

bool Datum::allowLookupByUID() const
{
    return !expr.empty() && (expr.front() == SIGIL_CONNECTION);
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
