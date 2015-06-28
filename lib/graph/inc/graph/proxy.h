#pragma once

#include <boost/python.hpp>

class Root;
class Node;
class Downstream;

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
    static PyObject* makeProxyFor(Root* r, Node* locals, Downstream* caller);
protected:
    Root* root;
    Node* locals;
    PyObject* dict;

    Downstream* caller;

    bool settable;

    static PyObject* proxy_init;
};
