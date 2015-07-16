#ifndef HOOKS_H
#define HOOKS_H

#include <Python.h>
#include <string>

#include "graph/hooks/external.h"

class Node;

class AppHooks : public ExternalHooks
{
public:
    struct Exception
    {
        Exception(std::string m) : message(m) {}
        std::string message;
    };

    static void onException(const Exception& e);
    static void preInit();

    /*
     *  Loads input, output, and title hooks into the given globals
     *  dictionary (with callbacks pointing to the given Node).
     */
    void load(PyObject* g, Node* n) override;
};

#endif
