#include <Python.h>
#include "graph/datum/wrapper.h"
#include "graph/datum/datums/script_datum.h"

static PyObject* ScriptInput_Call(PyObject* callable_object,
                                  PyObject* args, PyObject* kw)
{
    if (kw && PyDict_Size(kw) != 0)
    {
        PyErr_SetString(PyExc_RuntimeError,
                        "No keyword arguments allowed.");
        return NULL;
    }

    if (PyTuple_Size(args) != 2)
    {
        PyErr_SetString(PyExc_RuntimeError,
                        "Invalid argument count");
        return NULL;
    }

    PyObject* name = PyTuple_GetItem(args, 0);
    if (!PyUnicode_Check(name))
    {
        PyErr_SetString(PyExc_TypeError,
                        "Invalid first argument (must be a string)");
        return NULL;
    }

    PyObject* type = PyTuple_GetItem(args, 1);
    if (!PyType_Check(type))
    {
        PyErr_SetString(PyExc_TypeError,
                        "Invalid second argument (must be a type)");
        return NULL;
    }

    wchar_t* w = PyUnicode_AsWideCharString(name, NULL);
    Q_ASSERT(w);
    QString str = QString::fromWCharArray(w);
    PyMem_Free(w);

    ScriptDatum* d = ((ScriptInputWrapper*)callable_object)->datum;
    return d->makeInput(str, (PyTypeObject*)type);
}

static PyObject* ScriptOutput_Call(PyObject* callable_object,
                                   PyObject* args, PyObject* kw)
{
    if (kw && PyDict_Size(kw) != 0)
    {
        PyErr_SetString(PyExc_RuntimeError,
                        "No keyword arguments allowed.");
        return NULL;
    }

    if (PyTuple_Size(args) != 2)
    {
        PyErr_SetString(PyExc_RuntimeError,
                        "Invalid argument count");
        return NULL;
    }

    PyObject* name = PyTuple_GetItem(args, 0);
    if (!PyUnicode_Check(name))
    {
        PyErr_SetString(PyExc_TypeError,
                        "Invalid first argument (must be a string)");
        return NULL;
    }

    PyObject* out = PyTuple_GetItem(args, 1);

    wchar_t* w = PyUnicode_AsWideCharString(name, NULL);
    Q_ASSERT(w);
    QString str = QString::fromWCharArray(w);
    PyMem_Free(w);

    ScriptDatum* d = ((ScriptOutputWrapper*)callable_object)->datum;
    return d->makeOutput(str, out);
}

static PyObject* ScriptTitle_Call(PyObject* callable_object,
                                  PyObject* args, PyObject* kw)
{
    if (kw && PyDict_Size(kw) != 0)
    {
        PyErr_SetString(PyExc_RuntimeError,
                        "No keyword arguments allowed.");
        return NULL;
    }

    if (PyTuple_Size(args) != 1)
    {
        PyErr_SetString(PyExc_RuntimeError,
                        "Invalid argument count");
        return NULL;
    }

    PyObject* desc = PyTuple_GetItem(args, 0);
    if (!PyUnicode_Check(desc))
    {
        PyErr_SetString(PyExc_TypeError,
                        "Invalid first argument (must be a string)");
        return NULL;
    }

    wchar_t* w = PyUnicode_AsWideCharString(desc, NULL);
    Q_ASSERT(w);
    QString str = QString::fromWCharArray(w);
    PyMem_Free(w);

    ScriptDatum* d = ((ScriptOutputWrapper*)callable_object)->datum;
    return d->setTitle(str);
}

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
    ScriptInput_Call,          /* tp_call */
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
    ScriptOutput_Call,         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "Script output wrapper",   /* tp_doc */
};

static PyTypeObject ScriptTitleWrapperType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "wrapper.ScriptTitleWrapper",   /* tp_name */
    sizeof(ScriptTitleWrapper),     /* tp_basicsize */
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
    ScriptTitle_Call,          /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "Script title wrapper",    /* tp_doc */
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
    ScriptTitleWrapperType.tp_new = PyType_GenericNew;

    if (PyType_Ready(&ScriptInputWrapperType) < 0)
        return NULL;
    if (PyType_Ready(&ScriptOutputWrapperType) < 0)
        return NULL;
    if (PyType_Ready(&ScriptTitleWrapperType) < 0)
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
    PyModule_AddObject(m, "ScriptTitleWrapper",
                       (PyObject*)&ScriptTitleWrapperType);
    return m;
}

// Lazy initialization of the script wrapper module.
static PyObject* _wrapper_module = NULL;

PyObject* scriptInput(ScriptDatum* d)
{
    if (_wrapper_module == NULL)
        _wrapper_module = PyInit_wrapper();
    PyObject* p = PyObject_CallObject(
                PyObject_GetAttrString(_wrapper_module, "ScriptInputWrapper"),
                NULL);
    ((ScriptInputWrapper*)p)->datum = d;
    return p;
}

PyObject* scriptOutput(ScriptDatum* d)
{
    if (_wrapper_module == NULL)
        _wrapper_module = PyInit_wrapper();
    PyObject* p = PyObject_CallObject(
                PyObject_GetAttrString(_wrapper_module, "ScriptOutputWrapper"),
                NULL);
    ((ScriptOutputWrapper*)p)->datum = d;
    return p;
}

PyObject* scriptTitle(ScriptDatum* d)
{
    if (_wrapper_module == NULL)
        _wrapper_module = PyInit_wrapper();
    PyObject* p = PyObject_CallObject(
                PyObject_GetAttrString(_wrapper_module, "ScriptTitleWrapper"),
                NULL);
    ((ScriptTitleWrapper*)p)->datum = d;
    return p;
}
