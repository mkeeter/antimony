#include "graph/types/root.h"
#include "graph/datum.h"

#include <list>
#include <algorithm>

PyObject* Root::kwlist_contains = NULL;

Root::~Root()
{
    for (auto itr = lookups.begin(); itr != lookups.end(); ++itr)
        itr->second->roots.erase(this);
}

void Root::removeDownstream(Downstream* d)
{
    for (auto itr = lookups.begin(); itr != lookups.end();)
        if (itr->second == d)
            itr = lookups.erase(itr);
        else
            ++itr;
}

void Root::saveLookup(std::string name, Downstream* caller)
{
    lookups.insert(std::make_pair(name, caller));
    caller->roots.insert(this);
}

void Root::changed(std::string n, uint32_t uid)
{
    std::list<Downstream*> targets;
    for (auto s : {n, "__" + std::to_string(uid)})
    {
        auto range = lookups.equal_range(s);
        for (auto it = range.first; it != range.second; ++it)
            queue(it->second);
    }
    flushQueue();
}

bool Root::isNameValid(std::string name)
{
    static std::regex var("[_a-zA-Z][_a-zA-Z0-9]*");
    if (!std::regex_match(name, var))
        return false;

    // Lazy initialization of keyword.kwlist.__contains__
    if (!kwlist_contains)
    {
        PyObject* keyword_module = PyImport_ImportModule("keyword");
        PyObject* kwlist = PyObject_GetAttrString(keyword_module, "kwlist");
        Py_DECREF(keyword_module);
        kwlist_contains = PyObject_GetAttrString(kwlist, "__contains__");
        Py_DECREF(kwlist);
        assert(!PyErr_Occurred());
    }

    PyObject* in_kwlist = PyObject_CallFunction(
            kwlist_contains, "s", name.c_str());
    bool result = PyObject_IsTrue(in_kwlist);
    Py_DECREF(in_kwlist);
    assert(!PyErr_Occurred());

    return !result;
}
