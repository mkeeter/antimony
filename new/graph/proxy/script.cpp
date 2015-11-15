#include <Python.h>

#include "graph/proxy/script.h"
#include "graph/proxy/node.h"

#include "graph/script.h"

ScriptProxy::ScriptProxy(Script* s, NodeProxy* parent)
    : QObject(parent)
{
    s->installWatcher(this);
}

void ScriptProxy::trigger(const ScriptState& state)
{
    (void)state;
}
