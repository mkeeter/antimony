#include <Python.h>

#include "graph/proxy/graph.h"
#include "graph/proxy/datum.h"
#include "graph/proxy/node.h"

#include "canvas/connection/connection.h"
#include "app/colors.h"

Connection::Connection(const Datum* source, DatumProxy* target)
    : BaseConnection(Colors::getColor(source))
{
    // Get the GraphProxy in which our target datum lives
    GraphProxy* g = static_cast<GraphProxy*>(target->parent()->parent());

    auto p = g->getDatumProxy(source);
    Q_ASSERT(p != NULL);
    setPorts(p->outputPort(), target->inputPort());
}
