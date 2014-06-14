#include <Python.h>
#include "datum/wrapper.h"
#include "datum/script.h"

static PyTypeObject ScriptInputWrapperType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "wrapper.ScriptInputWrapper",   /* tp_name */
    sizeof(ScriptInputWrapper),     /* tp_basicsize */
    0,                         /* tp_itemsize */
    0,                         /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "Script input wrapper",    /* tp_doc */
};

static PyTypeObject ScriptOutputWrapperType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "wrapper.ScriptOutputWrapper",   /* tp_name */
    sizeof(ScriptOutputWrapper),     /* tp_basicsize */
    0,                         /* tp_itemsize */
    0,                         /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "Script output wrapper",   /* tp_doc */
};

static PyModuleDef ScriptWrapperModule = {
    PyModuleDef_HEAD_INIT,
    "wrapper",
    "Small module that contains script wrapper classes",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_wrapper(void)
{
    PyObject* m;

    ScriptInputWrapperType.tp_new = PyType_GenericNew;
    ScriptOutputWrapperType.tp_new = PyType_GenericNew;

    if (PyType_Ready(&ScriptInputWrapperType) < 0)
        return NULL;
    if (PyType_Ready(&ScriptOutputWrapperType) < 0)
        return NULL;

    m = PyModule_Create(&ScriptWrapperModule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&ScriptInputWrapperType);
    Py_INCREF(&ScriptOutputWrapperType);
    PyModule_AddObject(m, "ScriptInputWrapper",
                       (PyObject*)&ScriptInputWrapperType);
    PyModule_AddObject(m, "ScriptOutputWrapper",
                       (PyObject*)&ScriptOutputWrapperType);
    return m;
}

// Lazy initialization of the script wrapper module.
static PyObject* _wrapper_module = NULL;

PyObject* scriptInput(ScriptDatum* d)
{
    if (_wrapper_module == NULL)
    {
        _wrapper_module = PyInit_wrapper();
    }
    PyObject* p = PyObject_CallObject(
                PyObject_GetAttrString(_wrapper_module, "ScriptInputWrapper"),
                NULL);
    ((ScriptInputWrapper*)p)->datum = d;
    return p;
}

PyObject* scriptOutput(ScriptDatum* d)
{
    if (_wrapper_module == NULL)
    {
        _wrapper_module = PyInit_wrapper();
    }
    PyObject* p = PyObject_CallObject(
                PyObject_GetAttrString(_wrapper_module, "ScriptOutputWrapper"),
                NULL);
    ((ScriptOutputWrapper*)p)->datum = d;
    return p;
}
