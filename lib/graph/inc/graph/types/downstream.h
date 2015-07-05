#pragma once

#include <Python.h>
#include <unordered_set>

class Datum;
class Root;

class Downstream
{
public:
    virtual ~Downstream();
    virtual void update()=0;
protected:
    /*
     *  Returns true if a proxy lookup is allowed to use UIDs
     *  (which should only be true for datums with a connection sigil)
     */
    virtual bool allowLookupByUID() const=0;

    void trigger();

private:
    /*
     *  Stores roots that will re-evaluate this Datum when a particular name
     *  becomes available
     */
    std::unordered_set<Root*> roots;

    friend class Datum;
    friend class Root;
    friend class Node;
    friend class Proxy;
};
