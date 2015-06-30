#include "graph/types/downstream.h"
#include "graph/types/root.h"

Downstream::~Downstream()
{
    for (auto r : roots)
        r->removeDownstream(this);
}

void Downstream::trigger()
{
    sources.clear();
    for (auto r : roots)
        r->removeDownstream(this);
    update();
}
