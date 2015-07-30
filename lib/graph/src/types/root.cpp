#include "graph/types/root.h"
#include "graph/datum.h"
#include "graph/util.h"

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
    std::list<Downstream*> targets;
    for (auto s : {n, "__" + std::to_string(uid)})
    {
        auto range = lookups.equal_range(s);
        for (auto it = range.first; it != range.second; ++it)
            queue(it->second);
    }
    flushQueue();
}

bool Root::isNameValid(std::string name)
{
    static std::regex var("[_[:alpha:]][_[:alnum:]]*");
    if (!std::regex_match(name, var))
        return false;

    return !isPyKeyword(name);
}
