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

    std::string script;

    std::string output;
    std::string error;
    int error_lineno;

    std::unordered_set<Datum*> active;

protected:
    /*
     *  Never allow proxies to use UID lookups when a script
     *  is being evaluated.
     */
    bool allowLookupByUID() const override { return false; }

    Node* parent;
};
