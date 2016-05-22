#pragma once

#include <string>
#include <list>
#include <memory>

#include "graph/types/root.h"
#include "graph/types/watched.h"
#include "graph/script.h"
#include "graph/datum.h"
#include "graph/watchers.h"

class Datum;
class Graph;
class NodeWatcher;

class Node : public Root, public Watched<NodeWatcher, NodeState>
{
public:
    /*
     *  On construction, trigger parent Root's watchers if do_init is set.
     *
     *  If the given name ends with '*', it is treated as a wildcard prefix
     *  and Graph::nextName is called to get an appropriate name.
     */
    explicit Node(std::string name, Graph* root, bool do_init=true);
    explicit Node(std::string name, uint64_t uid,
                  Graph* root, bool do_init=true);

    /*
     *  Return the root pointer.
     */
    Graph* parentGraph() const { return parent; }

    /*
     *  Looks up a node by UID.
     */
    Datum* getDatum(uint32_t uid) const { return Root::getByUID(uid, datums); }

    /*
     *  Look up the node's name.
     */
    std::string getName() const { return name; }

    /*
     *  Look up fully qualified name
     *  (with dots separating nested graph levels)
     */
    std::string getFullName() const;

    /*
     *  Look up the node's UID.
     */
    uint64_t getUID() const { return uid; }

    /*
     *  Sets the node's name (and updates its parent graph)
     */
    void setName(std::string new_name);

    /*
     *  Returns the list of child datums.
     */
    std::list<Datum*> childDatums() const;

    /*
     *  Adds the given datum at the end of the list.
     *  Does NOT trigger NodeWatcher objects.
     *
     *  The node takes ownership of the datum and will delete it
     *  when the node is destroyed.
     */
    uint32_t install(Datum* d) { return Root::install(d, &datums); }

    /*
     *  Uninstalls the given datum.
     *  Does NOT trigger NodeWatcher objects.
     */
    void uninstall(Datum* d);

    /*
     *  Returns a mutable proxy for this node
     */
    PyObject* mutableProxy();

    /*
     *  Looks up a datum by name (or UID string).
     */
    Datum* getDatum(std::string name) const;

    /*
     *  Calls load on the parent graph's external hook object.
     */
    void loadDatumHooks(PyObject* g);

    /*
     *  Return the state (passed into callbacks)
     */
    NodeState getState() const override;

    /* Root functions */
    PyObject* pyGetAttr(std::string name, Downstream* caller,
                        uint8_t flags) const override;
    void pySetAttr(std::string name, PyObject* obj, uint8_t flags) override;
    void queue(Downstream* d) override;
    void flushQueue() override;

protected:
    /*
     *  Calls parent->changed and parent->triggerWatchers
     */
    void init();

    std::string name;
    const uint32_t uid;

    std::list<std::unique_ptr<Datum>> datums;
    Graph* parent;

    friend class Graph;
    friend class Datum;
    friend class Root;

    friend struct InputHook;
    friend struct OutputHook;
};

