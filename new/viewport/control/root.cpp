#include "viewport/control/root.h"
#include "viewport/control/control.h"

#include "graph/proxy/node.h"

ControlRoot::ControlRoot(NodeProxy* parent)
    : QObject(parent)
{
    // Nothing to do here
}

void ControlRoot::makeInstancesFor(ViewportView* v)
{
    for (auto c : controls)
    {
        c->makeInstanceFor(v);
    }
}
