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
    explicit Node(std::string name, Graph* root);
    explicit Node(std::string name, std::string script, Graph* root);

    /*
     *  On destruction, indicate that all datums have changed.
     */
    virtual ~Node();

    /*
     *  Return the root pointer.
     */
    Graph* parentGraph() const { return parent; }

    /*
     *  Look up the node's name.
     */
    std::string getName() const { return name; }

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
    void uninstall(Datum* d) { Root::uninstall(d, &datums); }

    /*
     *  Returns a Proxy object that uses the parent Graph as its
     *  root, this Node as its locals dictionary, and the given
     *  Downstream as the caller.
     */
    PyObject* proxyDict(Downstream* caller);

    /*
     *  Looks up a datum by name (or UID string).
     */
    Datum* getDatum(std::string name) const;

    /*
     *  Sets the callback object.
     */
    void installWatcher(NodeWatcher* w) { watchers.push_back(w); }

    /*
     *  Return the state (passed into callbacks)
     */
    NodeState getState() const;

    /* Root functions */
    PyObject* pyGetAttr(std::string name, Downstream* caller) const override;
    bool topLevel() const override { return false; }

protected:

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

    /*
    void makeOutput(std::string name, PyObject* out);
    */

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
