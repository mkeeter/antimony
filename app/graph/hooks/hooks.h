#pragma once

#include <Python.h>
#include <string>

#include "graph/hooks/external.h"

class Node;
class GraphProxy;

class AppHooks : public ExternalHooks
{
public:
    AppHooks(GraphProxy* g) : proxy(g) {}

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
    void loadScriptHooks(PyObject* g, ScriptNode* n) override;
    void loadDatumHooks(PyObject* g) override;

protected:
    GraphProxy* proxy;
};
