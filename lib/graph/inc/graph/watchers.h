#pragma once

#include <string>
#include <list>
#include <unordered_set>

class Datum;
class Node;
class Graph;

////////////////////////////////////////////////////////////////////////////////

struct DatumState
{
    std::string text;
    std::string repr;
    char sigil;
    bool editable;
    bool valid;
    std::string error;
    std::unordered_set<const Datum*> links;
};

class DatumWatcher
{
public:
    virtual ~DatumWatcher() {};
    virtual void trigger(const DatumState& state)=0;
};

////////////////////////////////////////////////////////////////////////////////

struct NodeState
{
    std::string script;
    std::string error;
    std::string output;
    int error_lineno;
    bool name_valid;
    std::list<Datum*> datums;
};

class NodeWatcher
{
public:
    virtual ~NodeWatcher() {};
    virtual void trigger(const NodeState& state)=0;
};

////////////////////////////////////////////////////////////////////////////////

struct GraphState
{
    std::unordered_set<Node*> nodes;
};

class GraphWatcher
{
public:
    virtual ~GraphWatcher() {};
    virtual void trigger(const GraphState& state)=0;
};
