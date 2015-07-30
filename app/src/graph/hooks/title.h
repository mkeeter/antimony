#ifndef HOOK_TITLE_H
#define HOOK_TITLE_H

#include <string>

class Node;
class GraphScene;

struct ScriptTitleHook
{
    ScriptTitleHook() : node(NULL), scene(NULL) {}
    void call(std::string name);

    Node* node;
    GraphScene* scene;
};

#endif
