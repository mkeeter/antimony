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
    void trigger();

private:
    /*
     *  Stores roots that will re-evaluate this Datum when a particular name
     *  becomes available
     */
    std::unordered_set<Root*> roots;

    /*
     *  This set represents any source whose modification could cause
     *  this source to be activated.  It is used to detect recursive loops.
     */
    std::unordered_set<Datum*> sources;

    friend class Datum;
    friend class Root;
    friend class Node;
};
