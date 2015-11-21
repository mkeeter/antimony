#pragma once

#include <string>

class NodeProxy;

struct ScriptTitleHook
{
    ScriptTitleHook() : proxy(NULL) {}
    void call(std::string title);

    NodeProxy* proxy;
};
