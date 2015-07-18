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

    friend class Node;
};
