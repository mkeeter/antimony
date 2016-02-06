#include <Python.h>

#include "graph/hooks/title.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/graph_scene.h"

void ScriptTitleHook::call(std::string name)
{
    scene->setTitle(node, QString::fromStdString(name));
}
