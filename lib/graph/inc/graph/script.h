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

    Node* parent;
};
