#pragma once

#include <Python.h>

#include <list>
#include <memory>

#include "graph/types/root.h"
#include "graph/node.h"
#include "graph/watchers.h"

#include "hooks/external.h"

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
     *  Returns a list of child nodes.
     */
    std::list<Node*> childNodes() const;

    /*
     *  Uninstall the given node.
     *  Triggers attached GraphWatcher objects.
     */
    void uninstall(Node* n);

    /*
     *  Uninstalls all attached nodes.
     */
    void clear();

    /*
     *  Looks up a node by UID.
     */
    Node* getNode(uint32_t uid) const { return Root::getByUID(uid, nodes); }

    /*
     *  Returns a Proxy object that uses this graph as its root,
     *  the given Node as its locals dictionary, and the given
     *  Downstream as the caller.
     */
    PyObject* proxyDict(Datum* caller);

    /*
     *  Sets and clears callback objects.
     */
    void installWatcher(GraphWatcher* w) { watchers.push_back(w); }
    void uninstallWatcher(GraphWatcher* w) { watchers.remove(w); }

    /*
     *  Triggers all of the connected GraphWatchers
     */
    void triggerWatchers() const;

    /*
     *  Return the state (used for callbacks)
     */
    GraphState getState() const;

    /*
     *  Checks that the given name is unique
     *  (or that the provided Node is the first instance of this name)
     */
    bool isNameUnique(std::string name, const Node* n=NULL) const;

    /*
     *  Returns the next unique name of the form prefix%i
     */
    std::string nextName(std::string prefix) const;

    /*
     *  Returns a list of n valid UIDs.
     */
    std::list<uint64_t> getUIDs(unsigned n) const;

    /*
     *  Installs an ExternalHooks object (used in dictionary generation)
     */
    void installExternalHooks(ExternalHooks* h) { external.reset(h); }

    /*
     *  Loads external hooks (if they are present)
     */
    void loadScriptHooks(PyObject* g, Node* n);
    void loadDatumHooks(PyObject* g);

    /* Root functions */
    PyObject* pyGetAttr(std::string name, Downstream* caller) const override;
    void pySetAttr(std::string, PyObject*) override {}
    void queue(Downstream* d) override;
    void flushQueue() override;

    /*
     *  Preloads Python modules.
     */
    static void preInit();

protected:
    std::string name;
    const uint32_t uid;

    Graph* parent;
    std::list<std::unique_ptr<Node>> nodes;

    std::list<GraphWatcher*> watchers;
    std::unique_ptr<ExternalHooks> external;

    bool processing_queue;
    std::unordered_set<Downstream*> downstream_queue;
};
