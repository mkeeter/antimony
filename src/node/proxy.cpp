#include "node/proxy.h"

static PyTypeObject proxy_ProxyType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "proxy.NodeProxy",               /* tp_name */
    sizeof(proxy_ProxyObject), /* tp_basicsize */
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
    "Node proxy objects",      /* tp_doc */
};

static PyModuleDef proxymodule = {
    PyModuleDef_HEAD_INIT,
    "proxy",
    "Small module that contains a node proxy class",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_proxy(void)
{
    PyObject* m;

    proxy_ProxyType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&proxy_ProxyType) < 0)
        return NULL;

    m = PyModule_Create(&proxymodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&proxy_ProxyType);
    PyModule_AddObject(m, "NodeProxy", (PyObject*)&proxy_ProxyType);
    return m;
}

////////////////////////////////////////////////////////////////////////////////

// Lazy initialization: when proxyType is first called, it calls PyInit_proxy()
static PyObject* _proxy_module = NULL;

PyObject* proxyType()
{
    if (_proxy_module == NULL)
    {
        _proxy_module = PyInit_proxy();
    }
    return PyObject_GetAttrString(_proxy_module, "NodeProxy");
}
