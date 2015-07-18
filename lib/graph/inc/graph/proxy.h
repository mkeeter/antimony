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
     *  Lookups will check the root, then locals, and will be marked as being
     *  made by the given Downstream object.
     *
     *  If the root is a top-level object, a dictionary will be constructed
     *  (for script evaluation, which may involve storing variables)
     *
     *  If an ExternalHooks pointer is provided, its 'load' function will
     *  be called (giving it the chance to inject variables into the
     *  dictionary)
     */
    static PyObject* makeProxyFor(Root* r, Node* locals, Downstream* caller,
                                  ExternalHooks* external=NULL,
                                  bool settable=false);

    /*
     *  Returns a new reference to the dict object of the given proxy
     *  (p must be a Proxy object).
     */
    static PyObject* getDict(PyObject* p);

protected:
    Root* root;
    Node* locals;
    PyObject* dict;

    Downstream* caller;

    bool settable;

    static PyObject* proxy_init;
};
