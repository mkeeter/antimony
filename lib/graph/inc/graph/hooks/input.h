#pragma once
#include <boost/python.hpp>

class Node;

struct InputHook
{
    InputHook() : node(NULL) {}
    InputHook(Node* n) : node(n) {}

    void call(std::string name, boost::python::object type);
    void call_with_default(std::string name, boost::python::object type,
                           boost::python::object default_value);

    Node* node;
};
