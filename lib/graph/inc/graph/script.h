#pragma once

#include <list>
#include <memory>
#include <string>

#include "graph/types/downstream.h"

class Datum;
class Node;

struct Script : public Downstream
{
public:
    Script(Node* parent);
    void update() override;
protected:
    /*
     *  Inject a variable into the globals dictionary.
     */
    void inject(std::string name, PyObject* value);

    /*
     *  Never allow proxies to use UID lookups when a script
     *  is being evaluated.
     */
    bool allowLookupByUID() const override { return false; }

    std::string script;
    std::string prev_script;

    std::string output;
    std::string error;
    int error_lineno;

    std::unordered_set<Datum*> active;
    Node* parent;
    PyObject* globals;

    friend class Node;
    friend class Graph;
};
