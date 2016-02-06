#include <Python.h>
#include "graph/graph_node.h"
#include "graph/graph.h"
#include "graph/proxy.h"

GraphNode::GraphNode(std::string name, Graph* root)
    : Node(name, root, false), subgraph(new Graph(this))
{
    init();
}

GraphNode::GraphNode(std::string name, uint32_t uid, Graph* root)
    : Node(name, uid, root, false), subgraph(new Graph(this))
{
    init();
}

bool GraphNode::makeDatum(std::string name, PyTypeObject* type,
                          bool output)
{
    // Construct a default datum of the given type
    auto obj = PyObject_CallObject((PyObject*)type, NULL);
    assert(!PyErr_Occurred());

    auto repr = PyObject_Repr(obj);
    assert(!PyErr_Occurred());

    auto value = std::string(PyUnicode_AsUTF8(repr));

    if (output)
        value = Datum::SIGIL_OUTPUT + value;

    auto d = new Datum(name, value, type, this);
    assert(d->isValid());

    Py_DECREF(obj);
    Py_DECREF(repr);

    bool out = d->isValid();
    triggerWatchers();
    subgraph->triggerWatchers();

    return out;
}

PyObject* GraphNode::pyGetAttr(std::string n, Downstream* caller,
                               uint8_t flags) const
{
    if (n == "__subgraph" && (flags & Proxy::FLAG_UID_LOOKUP))
        return Proxy::makeProxyFor(subgraph.get(), caller, flags);
    else
        return Node::pyGetAttr(n, caller, flags);
}
