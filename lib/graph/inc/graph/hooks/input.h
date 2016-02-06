#pragma once
#include <boost/python.hpp>

class ScriptNode;

struct InputHook
{
    InputHook() : node(NULL) {}
    InputHook(ScriptNode* n) : node(n) {}

    void call(std::string name, boost::python::object type);
    void call_with_default(std::string name, boost::python::object type,
                           boost::python::object default_value);

    ScriptNode* node;
};
