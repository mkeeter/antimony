#pragma once

#include <Python.h>

#include <list>
#include <memory>

#include "graph/types/root.h"
#include "graph/types/watched.h"
#include "graph/node.h"
#include "graph/watchers.h"

#include "hooks/external.h"

class Node;
class GraphNode;
class GraphWatcher;

class Graph : public Root, public Watched<GraphWatcher, GraphState>
{
public:
    explicit Graph(GraphNode* parent=NULL);

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
     *  Returns the parent node or NULL if this is a top-level graph.
     */
    GraphNode* parentNode() const { return parent; }

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
     *  Return the state (used for callbacks)
     */
    GraphState getState() const override;

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
     *
     *  The graph takes ownership of the hooks object and will delete it
     *  when the graph is deleted.
     */
    void installExternalHooks(ExternalHooks* h) { external.reset(h); }

    /*
     *  Loads external hooks (if they are present)
     */
    void loadScriptHooks(PyObject* g, ScriptNode* n);
    void loadDatumHooks(PyObject* g);

    /* Root functions */
    PyObject* pyGetAttr(std::string name, Downstream* caller,
                        uint8_t flags) const override;
    void pySetAttr(std::string, PyObject*, uint8_t) override;
    void queue(Downstream* d) override;
    void flushQueue() override;

    /*
     *  Preloads Python modules.
     */
    static void preInit();

protected:
    GraphNode* parent;

    std::list<std::unique_ptr<Node>> nodes;

    std::unique_ptr<ExternalHooks> external;

    bool processing_queue;
    std::unordered_set<Downstream*> downstream_queue;
};
