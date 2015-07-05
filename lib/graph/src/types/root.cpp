#include "graph/types/root.h"
#include "graph/datum.h"

#include <list>
#include <algorithm>

Root::~Root()
{
    for (auto itr = lookups.begin(); itr != lookups.end(); ++itr)
        itr->second->roots.erase(this);
}

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

void Root::changed(std::string n, uint32_t uid)
{
    std::unordered_set<Downstream*> targets;
    for (auto s : {n, "__" + std::to_string(uid)})
    {
        auto range = lookups.equal_range(s);
        for (auto it = range.first; it != range.second; ++it)
            targets.insert(it->second);
    }
    for (auto it : targets)
        it->trigger();
}

