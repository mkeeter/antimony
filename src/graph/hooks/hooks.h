#ifndef HOOKS_H
#define HOOKS_H

#include <Python.h>
#include <string>

class ScriptDatum;

namespace hooks {

    struct HookException
    {
        HookException(std::string m) : message(m) {}
        std::string message;
    };

    void onHookException(const HookException& e);
    void preInit();

    void loadHooks(PyObject* g, ScriptDatum* d);
}


#endif
