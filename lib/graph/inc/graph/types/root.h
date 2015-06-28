#pragma once
#include <Python.h>

#include <string>
#include <unordered_map>

class Downstream;

class Root
{
public:
    /*
     *  Returns a new reference to the given object or NULL.
     *  May throw Proxy::Exception to indicate a special error.
     */
    virtual PyObject* pyGetAttr(std::string name, Downstream* caller) const=0;

    /*
     *  Returns true if this root is a top-level object
     *  (changes whether Proxy populates dictionary and __builtins__)
     */
    virtual bool topLevel() const=0;

#if 0
    /*
     *  Checks that the given name is valid.
     */
    bool isNameValid(std::string name) const;

    /*
     *  Checks to see if the given name is unique.
     *  The input name should be stripped.
     */
    bool isNameUnique(std::string name) const;
#endif
    /*
     *  When a child is changed, call 'trigger' on all Downstream
     *  objects that have tried looking up this name before.
     */
    void changed(std::string n);

    /*
     *  Removes a Downstream from the map of failed lookups.
     */
    void removeDownstream(Downstream* d);

protected:
    std::unordered_multimap<std::string, Downstream*> lookups;
};
