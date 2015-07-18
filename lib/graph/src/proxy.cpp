#include "graph/proxy.h"
#include "graph/datum.h"
#include "graph/node.h"
#include "graph/types/root.h"
#include "graph/hooks/hooks.h"
#include "graph/hooks/external.h"

using namespace boost::python;

PyObject* Proxy::proxy_init = NULL;

Proxy::Proxy(Root* r)
    : root(r), locals(NULL), dict(NULL), caller(NULL), settable(false)
{
    // Nothing to do here
}

Proxy::Proxy()
    : root(NULL), locals(NULL), dict(NULL), caller(NULL), settable(false)
{
    // Nothing to do here
    // (but we need to set the root before this proxy can be used)
}

Proxy::~Proxy()
{
    Py_XDECREF(dict);
}

PyObject* Proxy::getAttr(std::string name)
{
    if (dict)
    {
        if (auto v = PyDict_GetItemString(dict, name.c_str()))
        {
            Py_INCREF(v);
            return v;
        }
        PyErr_Clear();

        // Look for this symbol in the builtins dictionary
        auto b = PyDict_GetItemString(dict, "__builtins__");
        if (auto v = PyDict_GetItemString(b, name.c_str()))
        {
            Py_INCREF(v);
            return v;
        }
        PyErr_Clear();
    }

    if (locals && caller)
        locals->saveLookup(name, caller);
    if (auto v = locals ? locals->pyGetAttr(name, caller) : NULL)
        return v;

    if (caller)
        root->saveLookup(name, caller);
    if (auto v = root->pyGetAttr(name, caller))
        return v;

    throw Exception("Name '" + name + "' is not defined");
    return NULL;
}

void Proxy::setAttr(std::string name, object obj)
{
    if (dict)
    {
        PyDict_SetItemString(dict, name.c_str(), obj.ptr());
        return;
    }

    if (!settable)
        throw Proxy::Exception("Cannot set value with non-mutable Proxy");

    root->pySetAttr(name, obj.ptr());
}

PyObject* Proxy::makeProxyFor(Root* r, Node* locals, Downstream* caller,
                              ExternalHooks* external, bool settable)
{
    // Get Python object constructor (with lazy initialization)
    if (proxy_init == NULL)
    {
        auto _proxy_module = PyImport_ImportModule("_proxy");
        proxy_init = PyObject_GetAttrString(_proxy_module, "Proxy");
        if (PyErr_Occurred())
            PyErr_Print();
        Py_DECREF(_proxy_module);
    }

    auto p = PyObject_CallObject(proxy_init, NULL);
    assert(!PyErr_Occurred());

    auto ex = extract<Proxy*>(p);
    assert(ex.check());
    auto p_ = ex();

    p_->root = r;
    p_->locals = locals;
    p_->caller = caller;
    p_->settable = settable;
    if (r->topLevel())
    {
        p_->dict = PyDict_New();

        PyDict_SetItemString(p_->dict, "__builtins__", PyEval_GetBuiltins());
        PyDict_SetItemString(p_->dict, "math", PyImport_ImportModule("math"));

        Hooks::load(p_->dict, locals);
        if (external)
            external->load(p_->dict, locals);
    }
    return p;
}

PyObject* Proxy::getDict(PyObject* p)
{
    PyObject* d = boost::python::extract<Proxy&>(p)().dict;
    assert(d);
    Py_INCREF(d);
    return d;
}

////////////////////////////////////////////////////////////////////////////////

void Proxy::onException(const Proxy::Exception& e)
{
    PyErr_SetString(PyExc_RuntimeError, e.message.c_str());
}

BOOST_PYTHON_MODULE(_proxy)
{
    class_<Proxy>("Proxy", init<>())
        .def("__getattr__", &Proxy::getAttr)
        .def("__getitem__", &Proxy::getAttr)
        .def("__setattr__", &Proxy::setAttr)
        .def("__setitem__", &Proxy::setAttr);

    register_exception_translator<Proxy::Exception>(
            Proxy::onException);
}

void Proxy::preInit()
{
    PyImport_AppendInittab("_proxy", PyInit__proxy);
}

