#include "graph/types/downstream.h"
#include "graph/types/root.h"

Downstream::~Downstream()
{
    for (auto r : roots)
        r->removeDownstream(this);
}

void Downstream::trigger()
{
#if 0
    sources.clear();
    sources.insert(this);
#endif

    for (auto r : roots)
        r->removeDownstream(this);
    upstream.clear();

    update();
}
