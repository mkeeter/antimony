#include "graph/proxy.h"
#include "graph/datum.h"
#include "graph/node.h"
#include "graph/types/root.h"

using namespace boost::python;

PyObject* Proxy::proxy_init = NULL;

Proxy::Proxy(Root* r)
    : root(r), caller(NULL), globals(NULL), flags(0)
{
    // Nothing to do here
}

Proxy::Proxy()
    : root(NULL), caller(NULL), globals(NULL), flags(0)
{
    // Nothing to do here
    // (but we need to set the root before this proxy can be used)
}

Proxy::~Proxy()
{
    // Nothing to do here
}

PyObject* Proxy::getAttr(std::string name)
{
    // If we're given a globals dictionary, then use it for this lookup
    // (checking in both the dictionary itself and the __builtins__ subdict)
    if (globals)
    {
        if (auto v = PyDict_GetItemString(globals, name.c_str()))
        {
            Py_INCREF(v);
            return v;
        }
        PyErr_Clear();

        // Look for this symbol in the builtins dictionary
        auto b = PyDict_GetItemString(globals, "__builtins__");
        if (auto v = PyDict_GetItemString(b, name.c_str()))
        {
            Py_INCREF(v);
            return v;
        }
        PyErr_Clear();
    }

    if (caller)
        root->saveLookup(name, caller);
    if (auto v = root->pyGetAttr(name, caller, flags))
        return v;

    throw Exception("Name '" + name + "' is not defined");
    return NULL;
}

void Proxy::setAttr(std::string name, object obj)
{
    if (!(flags & FLAG_MUTABLE))
        throw Proxy::Exception("Cannot set value with non-mutable Proxy");
    root->pySetAttr(name, obj.ptr(), flags);
}

PyObject* Proxy::makeProxyFor(Root* r, Downstream* caller, uint8_t flags)
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
    p_->caller = caller;
    p_->flags = flags;
    return p;
}

void Proxy::setGlobals(PyObject* proxy, PyObject* globals)
{
    boost::python::extract<Proxy*>(proxy)()->globals = globals;
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

