#pragma once

#include <Python.h>

#include <list>
#include <memory>

#include "graph/types/root.h"
#include "graph/watchers.h"

class Node;
class GraphWatcher;

class Graph : public Root
{
public:
    explicit Graph(std::string name="", Graph* parent=NULL);

    /*
     *  Installs this node at the end of the node list.
     *  Triggers attached GraphWatcher objects.
     *
     *  The graph takes ownership of the node and will delete
     *  it when the graph is destroyed.
     *
     *  Returns a unique ID number.
     */
    uint32_t install(Node* n);

    /*
     *  Uninstall the given node.
     *  Triggers attached GraphWatcher objects.
     */
    void uninstall(Node* n);

    /*
     *  Returns a Proxy object that uses this graph as its root,
     *  the given Node as its locals dictionary, and the given
     *  Downstream as the caller.
     */
    PyObject* proxyDict(Node* locals, Downstream* caller);

    /*
     *  Sets the callback object.
     */
    void installWatcher(GraphWatcher* w) { watchers.push_back(w); }

    /*
     *  Triggers all of the connected GraphWatchers
     */
    void triggerWatchers() const;

    /*
     *  Return the state (used for callbacks)
     */
    GraphState getState() const;

    /* Root functions */
    bool topLevel() const override { return parent == NULL; }
    PyObject* pyGetAttr(std::string name, Downstream* caller) const override;

    /*
     *  Preloads Python modules.
     */
    static void preInit();

protected:
    //void pySetAttr(std::string name, PyObject* obj) override;

    std::string name;
    const uint32_t uid;

    Graph* parent;
    std::list<std::unique_ptr<Node>> nodes;

    std::list<GraphWatcher*> watchers;
};
