#pragma once

#include <list>
#include <memory>
#include <string>

#include "graph/types/downstream.h"

class Datum;
class Node;

class Script : public Downstream
{
public:
    void update() override;
protected:
    std::string script;

    std::string output;
    PyObject* globals;

    std::unordered_set<Datum*> active;

    Node* parent;

    friend class Node;
};
