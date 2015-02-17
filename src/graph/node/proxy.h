#ifndef NODE_PROXY_H
#define NODE_PROXY_H

#include <Python.h>

class Datum;
class Node;

struct NodeProxy
{
    NodeProxy() : node(NULL), caller(NULL) {}
    PyObject* getAttr(std::string name);

    Node* node;
    Datum* caller;
};

namespace proxy {
    struct ProxyException {
        ProxyException(std::string m) : message(m) {}
        std::string message;
    };
    void onProxyException(const ProxyException& e);
    void preInit();
};

#endif // NODE_PROXY_H
