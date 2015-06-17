#ifndef NODE_PROXY_H
#define NODE_PROXY_H

#include <boost/python.hpp>

class Datum;
class Node;

struct NodeProxy
{
    NodeProxy() : node(NULL), caller(NULL), settable(false) {}
    PyObject* getAttr(std::string name);
    void setAttr(std::string name, boost::python::object obj);

    Node* node;
    Datum* caller;
    bool settable;
};

namespace proxy {
    struct ProxyException {
        ProxyException(std::string m) : message(m) {}
        std::string message;
    };
    void onProxyException(const ProxyException& e);
    void preInit();
    PyObject* proxyConstructor();

    extern PyObject* proxy_init;
};

#endif // NODE_PROXY_H
