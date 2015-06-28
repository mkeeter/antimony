#pragma once

#include <string>
#include <list>

#include "graph/types/root.h"
#include "graph/script.h"

class Datum;
class Graph;

class Node : public Root
{
public:
    explicit Node(std::string name, Graph* root);

    /*
     *  Adds the given datum at the end of the list.
     */
    void install(Datum* d);

    /*
     *  Returns a Proxy object that uses the parent Graph as its
     *  root, this Node as its locals dictionary, and the given
     *  Downstream as the caller.
     */
    PyObject* proxyDict(Downstream* caller);

    Datum* getDatum(std::string name) const;

    /* Root functions */
    PyObject* pyGetAttr(std::string name, Downstream* caller) const override;
    bool topLevel() const override { return false; }

protected:

    void makeInput(std::string name, PyTypeObject* type,
                   std::string value);
    /*
    void makeOutput(std::string name, PyObject* out);
    */

    void pruneInactive(std::unordered_set<Datum*> active);

    std::string name;

    Script script;
    std::list<Datum*> datums;
    Graph* parent;

    friend class Graph;
    friend class Script;
    friend class Datum;
};
