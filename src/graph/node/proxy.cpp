#include <boost/python.hpp>

#include <QString>

#include "graph/datum/datum.h"
#include "graph/node/node.h"
#include "graph/node/proxy.h"

using namespace boost::python;

#include <QDebug>
PyObject* NodeProxy::getAttr(std::string name)
{
    Datum* datum = node->getDatum(QString::fromStdString(name));

    if (datum)
    {
        // If we have a known caller, then mark that this datum is an upstream node
        // for the caller.
        bool failed = false;
        if (caller)
        {
            // Try to connect this datum as an upstream datum of the caller
            if (!caller->connectUpstream(datum))
                failed = true;

            // Also connect the node's name as an upstream datum
            // (since if the name changes, the expression may become invalid)
            Node* n = dynamic_cast<Node*>(datum->parent());
            Q_ASSERT(n);
            auto name = n->getDatum("_name");
            Q_ASSERT(name);
            caller->connectUpstream(name);
        }

        if (failed)
        {
            throw proxy::ProxyException("Recursive loop in lookup.");
        }
        else if (datum->getValid())
        {
            PyObject* value = datum->getValue();
            Py_INCREF(value);
            return value;
        }
        else
        {
            throw proxy::ProxyException("Invalid datum lookup.");
        }
    }
    else
    {
        throw proxy::ProxyException("Nonexistent datum lookup.");
    }
}

void proxy::onProxyException(const proxy::ProxyException& e)
{
    PyErr_SetString(PyExc_RuntimeError, e.message.c_str());
}

BOOST_PYTHON_MODULE(_proxy)
{
    class_<NodeProxy>("NodeProxy", init<>())
        .def("__getattr__", &NodeProxy::getAttr);

    register_exception_translator<proxy::ProxyException>(
            proxy::onProxyException);
}

void proxy::preInit()
{
    PyImport_AppendInittab("_proxy", PyInit__proxy);
}
