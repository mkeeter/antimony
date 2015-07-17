#ifndef HOOK_TITLE_H
#define HOOK_TITLE_H

#include <boost/python.hpp>

class ScriptDatum;
class NodeInspector;

struct ScriptTitleHook
{
    ScriptTitleHook() : inspector(NULL) {}
    void call(std::string name);

    NodeInspector* inspector;
};

#endif
