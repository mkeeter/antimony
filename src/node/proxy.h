#ifndef NODE_PROXY_H
#define NODE_PROXY_H

#include <Python.h>

class Datum;
class Node;

typedef struct {
    PyObject_HEAD
    Datum* caller;
    Node* node;
} proxy_ProxyObject;

PyObject* proxyType();

#endif // NODE_PROXY_H
