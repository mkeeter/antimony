#ifndef WRAPPER_H
#define WRAPPER_H

#include <Python.h>

class ScriptDatum;

typedef struct {
    PyObject_HEAD
    ScriptDatum* datum;
} ScriptWrapperObject;

#endif // WRAPPER_H
