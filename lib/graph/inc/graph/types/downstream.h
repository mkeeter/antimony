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
     *  This set stores Values that are directly connected to us.
     */
    std::unordered_set<Datum*> upstream;

    /*
     *  Stores roots that will re-evaluate this Datum when a particular name
     *  becomes available
     */
    std::unordered_set<Root*> roots;
#if 0
    /*
     *  This set represents any source whose modification could cause
     *  this source to be activated.  It is used to detect recursive loops.
     */
    std::unordered_set<Downstream*> sources;
#endif

    friend class Datum;
    friend class Root;
};
