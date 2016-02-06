#pragma once

#include <boost/python.hpp>

class Root;
class Node;
class Downstream;
class ExternalHooks;

class Proxy
{
public:
    /*
     *  This constructor should only be used by Python
     *  (wrapped so that we populate the root afterwards).
     */
    Proxy();
    Proxy(Root* r);

    /*
     *  On destruction, delete the Python dictionary
     */
    ~Proxy();

    PyObject* getAttr(std::string s);
    void setAttr(std::string name, boost::python::object obj);

    struct Exception {
        Exception(std::string m) : message(m) {}
        std::string message;
    };
    static void onException(const Exception& e);
    static void preInit();

    /*
     *  Constructs a Proxy dictionary for the given root.
     *
     *  Lookups will check the root and will be marked as being made by the
     *  given Downstream object (using root->saveLookup)
     */
    static PyObject* makeProxyFor(Root* r, Downstream* caller, uint8_t flags=0);

    /*
     *  Sets the globals dictionary in a proxy object.
     */
    static void setGlobals(PyObject* proxy, PyObject* globals);

    // These are potential flags for the proxy object
    const static uint8_t FLAG_MUTABLE  = 1;
    const static uint8_t FLAG_UID_LOOKUP = 2;

protected:
    Root* root;
    Downstream* caller;
    PyObject* globals;

    uint8_t flags;

    static PyObject* proxy_init;
};
