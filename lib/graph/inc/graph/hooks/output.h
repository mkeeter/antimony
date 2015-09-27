#pragma once
#include <boost/python.hpp>

class ScriptNode;

struct OutputHook
{
    OutputHook() : node(NULL) {}
    OutputHook(ScriptNode* n) : node(n) {}

    void call(std::string name, boost::python::object obj);

    ScriptNode* node;
};

