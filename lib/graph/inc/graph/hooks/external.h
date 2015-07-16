#pragma once

#include <Python.h>

class Node;

class ExternalHooks
{
public:
    virtual ~ExternalHooks() {}
    virtual void load(PyObject* g, Node* n)=0;
};
