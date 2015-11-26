#pragma once

#include <list>
#include <memory>
#include <string>

#include "graph/types/downstream.h"
#include "graph/types/watched.h"
#include "graph/watchers.h"

class Datum;
class ScriptNode;

struct Script : public Downstream, public Watched<ScriptWatcher, ScriptState>
{
public:
    Script(ScriptNode* parent);
    void update() override;

    ScriptState getState() const override;

    /*
     *  Sets the script's text and calls trigger()
     */
    void setText(std::string t);

    /*
     *  Returns the parent node
     */
    ScriptNode* parentNode() const { return parent; }

protected:
    /*
     *  Inject a variable into the globals dictionary.
     */
    void inject(std::string name, PyObject* value);

    std::string script;
    std::string prev_script;

    std::string output;
    std::string error;
    int error_lineno;

    std::unordered_set<Datum*> active;
    ScriptNode* parent;
    PyObject* globals;

    friend class ScriptNode;
    friend class Graph;
};
