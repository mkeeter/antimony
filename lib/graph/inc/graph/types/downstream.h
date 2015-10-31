#pragma once

#include <Python.h>

#include <unordered_set>

class Datum;
class Root;

class Downstream
{
public:
    virtual ~Downstream();

    /*
     *  Virtual function that is called by trigger()
     *  (after trigger sets up root tracking)
     *
     *  Derived classes are responsible for updating sources.
     */
    virtual void update()=0;

protected:

    void trigger();

    /*
     *  This set represents any Downstream object that when triggered could
     *  cause this Downstream to be triggered.
     *
     *  It is used to detect recursive loops and schedule queued evaluation.
     */
    std::unordered_set<const Downstream*> sources;

private:
    /*
     *  Stores roots that will re-evaluate this Datum when a particular name
     *  becomes available
     */
    std::unordered_set<Root*> roots;

    friend class Root;
    friend class Graph;
};
