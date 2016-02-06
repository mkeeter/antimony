#pragma once

#include <Python.h>

class ScriptNode;

class ExternalHooks
{
public:
    virtual ~ExternalHooks() {}

    /*
     *  Load hooks appropriate for calling a script
     */
    virtual void loadScriptHooks(PyObject* g, ScriptNode* n)=0;

    /*
     *  Load hooks appropriate for evaluating a datum
     */
    virtual void loadDatumHooks(PyObject* g)=0;
};
