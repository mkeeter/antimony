#pragma once

#include <string>
#include <list>

class Datum;

struct DatumState
{
    std::string text;
    bool editable;
    bool valid;
    std::string error;
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
    int error_lineno;
    std::list<Datum*> datums;
};

class NodeWatcher
{
public:
    virtual ~NodeWatcher() {};
    virtual void trigger(const NodeState& state)=0;
};

