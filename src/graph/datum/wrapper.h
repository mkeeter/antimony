#ifndef WRAPPER_H
#define WRAPPER_H

#include <Python.h>

class ScriptDatum;

typedef struct {
    PyObject_HEAD
    ScriptDatum* datum;
} ScriptInputWrapper;

typedef struct {
    PyObject_HEAD
    ScriptDatum* datum;
} ScriptOutputWrapper;

typedef struct {
    PyObject_HEAD
    ScriptDatum* datum;
} ScriptTitleWrapper;

PyObject* scriptInput(ScriptDatum* d);
PyObject* scriptOutput(ScriptDatum* d);
PyObject* scriptTitle(ScriptDatum* d);

#endif // WRAPPER_H
