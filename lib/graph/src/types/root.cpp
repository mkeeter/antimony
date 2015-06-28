#include "graph/types/root.h"
#include "graph/datum.h"

void Root::removeDownstream(Downstream* d)
{
    for (auto itr = lookups.begin(); itr != lookups.end();)
        if (itr->second == d)
            itr = lookups.erase(itr);
        else
            ++itr;
}

void Root::changed(std::string n)
{
    auto range = lookups.equal_range(n);
    for (auto it = range.first; it != range.second; ++it)
        it->second->trigger();
}

