#ifndef HOOK_TITLE_H
#define HOOK_TITLE_H

#include <boost/python.hpp>

class ScriptDatum;

struct ScriptTitleHook
{
    ScriptTitleHook() : datum(NULL) {}
    ScriptTitleHook(ScriptDatum* d) : datum(d) {}

    void call(std::string name);

    ScriptDatum* datum;
};

#endif
