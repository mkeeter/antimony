#pragma once

#include <Python.h>

#include <list>
#include <memory>

#include "graph/types/root.h"

class Node;

class Graph : public Root
{
public:
    explicit Graph(std::string name="", Graph* parent=NULL);

    /*
     *  Installs this node at the end of the node list.
     *
     *  The graph takes ownership of the node and will delete
     *  it when the graph is destroyed.
     *
     *  Returns a unique ID number.
     */
    uint32_t install(Node* n) { return Root::install(n, &nodes); }

    /*
     *  Uninstall the given node.
     */
    void uninstall(Node* n) { Root::uninstall(n, &nodes); }

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
    const uint32_t uid;

    Graph* parent;
    std::list<std::unique_ptr<Node>> nodes;
};
