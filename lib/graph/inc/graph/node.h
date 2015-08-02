#pragma once

#include <string>
#include <list>
#include <memory>

#include "graph/types/root.h"
#include "graph/script.h"
#include "graph/datum.h"
#include "graph/watchers.h"

class Datum;
class Graph;
class NodeWatcher;

class Node : public Root
{
public:
    /*
     *  On construction, trigger parent Root's watchers.
     *
     *  If the given name ends with '*', it is treated as a wildcard prefix
     *  and Graph::nextName is called to get an appropriate name.
     */
    explicit Node(std::string name, Graph* root);
    explicit Node(std::string name, std::string script, Graph* root);
    explicit Node(std::string name, uint64_t uid, Graph* root);

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
     *  Updates the script text and triggers an update.
     */
    void setScript(std::string t);

    /*
     *  Looks up the current text of the script.
     */
    std::string getScript() const { return script.script; }

    /*
     *  Returns the error message from script evaluation.
     */
    std::string getError() const { return script.error; }

    /*
     *  Returns the error line from script evaluation.
     *  (indexed from 1)
     */
    int getErrorLine() const { return script.error_lineno; }

    /*
     *  When the script is done running, update the node
     *  (which may require removing datums from the list)
     */
    void update(const std::unordered_set<Datum*>& active);

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
     *  Returns a Proxy object that uses the parent Graph as its
     *  root and the given datum as the caller (for datum evaluation)
     */
    PyObject* proxyDict(Datum* caller);

    /*
     *  Returns a mutable proxy for this node
     */
    PyObject* mutableProxy();

    /*
     *  Looks up a datum by name (or UID string).
     */
    Datum* getDatum(std::string name) const;

    /*
     *  Sets the callback object.
     */
    void installWatcher(NodeWatcher* w) { watchers.push_back(w); }
    void uninstallWatcher(NodeWatcher* w);

    /*
     *  Calls load on the parent graph's external hook object.
     */
    void loadScriptHooks(PyObject* g);
    void loadDatumHooks(PyObject* g);

    /*
     *  Return the state (passed into callbacks)
     */
    NodeState getState() const;

    /* Root functions */
    PyObject* pyGetAttr(std::string name, Downstream* caller) const override;
    void pySetAttr(std::string name, PyObject* obj) override;
    void queue(Downstream* d) override;
    void flushQueue() override;

protected:

    /*
     *  Calls parent->changed and parent->triggerWatchers
     */
    void init();

    /*
     *  Construct a datum with the given name, type, and value.
     *
     *  If output is false, then only load the value on new datum
     *  construction (as a default); if output is true, then load
     *  the value regardless of whether the datum is new or existing.
     *
     *  Returns true on success, false if the datum was already created
     *  in this pass through the script (checked using script.active).
     */
    bool makeDatum(std::string name, PyTypeObject* type,
                   std::string value, bool output);

    void pruneInactive(std::unordered_set<Datum*> active);

    std::string name;
    const uint32_t uid;

    Script script;
    std::list<std::unique_ptr<Datum>> datums;
    Graph* parent;

    std::list<NodeWatcher*> watchers;

    friend class Graph;
    friend class Datum;
    friend class Root;
    friend struct Script;

    friend struct InputHook;
    friend struct OutputHook;
};
