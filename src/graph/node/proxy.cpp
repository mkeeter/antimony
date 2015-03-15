#include <boost/python.hpp>

#include <QString>

#include "graph/datum/datum.h"
#include "graph/datum/types/eval_datum.h"
#include "graph/node/node.h"
#include "graph/node/proxy.h"

using namespace boost::python;

void NodeProxy::setAttr(std::string name, object obj)
{
    if (!settable)
        throw proxy::ProxyException("Cannot set datum value.");

    Datum* datum = node->getDatum(QString::fromStdString(name));

    if (!datum)
        throw proxy::ProxyException("Nonexistent datum lookup.");

    if (obj.ptr()->ob_type != datum->getType())
        throw proxy::ProxyException("Invalid type.");

    auto e = dynamic_cast<EvalDatum*>(datum);
    if (!e)
        throw proxy::ProxyException("Datum must be an EvalDatum");

    // Make sure that the existing expression can be directly coerced into
    // a value of the desired type.  This is so that assigning x when
    // x equals "12.0" works, but assigning x when x = "po.y" fails.
    auto o = PyObject_CallFunction((PyObject*)datum->getType(), "s",
                                    e->getExpr().toStdString().c_str());
    if (!o)
    {
        PyErr_Clear();
        return;
    }
    Py_DECREF(o);

    auto txt = QString::fromStdString(extract<std::string>(str(obj))());

    // Special case: use QString::number to sanely trim the number of decimal
    // places that are printed for float values.
    if (datum->getType() == &PyFloat_Type)
        txt = QString::number(txt.toFloat());

    e->setExpr(txt);
}

PyObject* NodeProxy::getAttr(std::string name)
{
    Datum* datum = node->getDatum(QString::fromStdString(name));

    if (!datum)
        throw proxy::ProxyException("Nonexistent datum lookup.");

    // If we have a known caller, then mark that this datum is an upstream node
    // for the caller.
    if (caller)
    {
        // Try to connect this datum as an upstream datum of the caller
        if (!caller->connectUpstream(datum))
            throw proxy::ProxyException("Recursive loop in lookup.");

        // Also connect the node's name as an upstream datum
        // (since if the name changes, the expression may become invalid)
        Q_ASSERT(dynamic_cast<Node*>(datum->parent()));
        Node* n = static_cast<Node*>(datum->parent());

        auto name = n->getDatum("__name");
        Q_ASSERT(name);
        caller->connectUpstream(name);
    }

    if (!datum->getValid())
        throw proxy::ProxyException("Invalid datum lookup.");

    PyObject* value = datum->getValue();
    Py_INCREF(value);
    return value;
}

void proxy::onProxyException(const proxy::ProxyException& e)
{
    PyErr_SetString(PyExc_RuntimeError, e.message.c_str());
}

BOOST_PYTHON_MODULE(_proxy)
{
    class_<NodeProxy>("NodeProxy", init<>())
        .def("__getattr__", &NodeProxy::getAttr)
        .def("__setattr__", &NodeProxy::setAttr);

    register_exception_translator<proxy::ProxyException>(
            proxy::onProxyException);
}

void proxy::preInit()
{
    PyImport_AppendInittab("_proxy", PyInit__proxy);
}

PyObject* proxy::proxy_init = NULL;

PyObject* proxy::proxyConstructor()
{
    if (proxy_init == NULL)
    {
        auto _proxy_module = PyImport_ImportModule("_proxy");
        proxy_init = PyObject_GetAttrString(_proxy_module, "NodeProxy");
        Py_DECREF(_proxy_module);
    }
    return proxy_init;
}
