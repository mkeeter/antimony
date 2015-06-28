#pragma once

#include <Python.h>

#include <list>

#include "graph/types/root.h"

class Node;

class Graph : public Root
{
public:
    explicit Graph(std::string name="", Graph* parent=NULL);

    /*
     *  Installs this datum at the end of the datum list.
     */
    void install(Node* n);

    /*
     *  Returns a Proxy object that uses this graph as its root,
     *  the given Node as its locals dictionary, and the given
     *  Downstream as the caller.
     */
    PyObject* proxyDict(Node* locals, Downstream* caller);

    /* Root functions */
    bool topLevel() const override { return parent == NULL; }
    PyObject* pyGetAttr(std::string name, Downstream* caller) const override;

protected:
    //void pySetAttr(std::string name, PyObject* obj) override;

    std::string name;
    Graph* parent;
    std::list<Node*> nodes;
};
