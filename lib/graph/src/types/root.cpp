#include "graph/types/root.h"
#include "graph/datum.h"

#include <list>
#include <algorithm>

void Root::removeDownstream(Downstream* d)
{
    for (auto itr = lookups.begin(); itr != lookups.end();)
        if (itr->second == d)
            itr = lookups.erase(itr);
        else
            ++itr;
}

void Root::saveLookup(std::string name, Downstream* caller)
{
    lookups.insert(std::make_pair(name, caller));
    caller->roots.insert(this);
}

void Root::changed(std::string n)
{
    auto range = lookups.equal_range(n);
    std::list<Downstream*> targets;
    for (auto it = range.first; it != range.second; ++it)
        targets.push_back(it->second);
    for (auto it : targets)
        it->trigger();
}

