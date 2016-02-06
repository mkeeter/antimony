#include "graph/types/downstream.h"
#include "graph/types/root.h"

Downstream::~Downstream()
{
    for (auto r : roots)
        r->removeDownstream(this);
}

void Downstream::trigger()
{
    for (auto r : roots)
        r->removeDownstream(this);
    roots.clear();

    update();
}
