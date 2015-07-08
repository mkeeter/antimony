#pragma once
#include <boost/python.hpp>

class Node;

struct OutputHook
{
    OutputHook() : node(NULL) {}
    OutputHook(Node* n) : node(n) {}

    void call(std::string name, boost::python::object obj);

    Node* node;
};

