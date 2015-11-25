#pragma once

#include "graph/node.h"

class ScriptNode : public Node
{
public:
    explicit ScriptNode(std::string name, Graph* root);
    explicit ScriptNode(std::string name, uint32_t uid, Graph* root);
    explicit ScriptNode(std::string name, std::string script, Graph* root);

    /*
     *  Returns the script's state (error condition, error line, etc)
     */
    ScriptState getScriptState() const;

    /*
     *  Install a watcher object on the node's script.
     */
    void installScriptWatcher(ScriptWatcher* w) { script.installWatcher(w); }
    void uninstallScriptWatcher(ScriptWatcher* w) { script.uninstallWatcher(w); }

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
     *  Get pointer to script object
     */
    Script* getScriptPointer() const { return const_cast<Script*>(&script); }

    /*
     *  Constructs a set of script hooks pointing to this node.
     */
    void loadScriptHooks(PyObject* g);

protected:
    /*
     *  When the script is done running, update the node
     *  (which may require removing datums from the list)
     */
    void update(const std::unordered_set<Datum*>& active);

    Script script;

    friend struct Script;
};
