#pragma once

#include <Python.h>
#include <string>

class Node;

namespace Hooks {

    struct Exception
    {
        Exception(std::string m) : message(m) {}
        std::string message;
    };

    void checkName(std::string name);

    void onException(const Exception& e);
    void preInit();

    // Populate script IO hooks for the given node.
    void load(PyObject* g, Node* n);
};
