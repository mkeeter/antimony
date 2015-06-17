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

    /*
     *  Loads input, output, and title hooks into the given globals
     *  dictionary (with callbacks pointing to the given datum).
     *
     *  Returns the object that used to be in fab.ui
     *  (so that it can be restored, making nested calls work)
     */
    PyObject* loadHooks(PyObject* g, ScriptDatum* d);
}


#endif
