#pragma once

#include <string>
#include <list>
#include <memory>

#include "graph/types/root.h"
#include "graph/script.h"
#include "graph/datum.h"

class Datum;
class Graph;

class Node : public Root
{
public:
    explicit Node(std::string name, Graph* root);

    /*
     *  Updates the script text and triggers an update.
     */
    void setScript(std::string t);

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
     *
     *  The node takes ownership of the datum and will delete it
     *  when the node is destroyed.
     */
    uint32_t install(Datum* d);

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
    const uint32_t uid;

    Script script;
    std::list<std::unique_ptr<Datum>> datums;
    Graph* parent;

    friend class Graph;
    friend class Datum;
    friend class Root;
    friend struct Script;
    friend struct InputHook;
};
