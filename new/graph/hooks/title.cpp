#include <Python.h>

#include "graph/hooks/title.h"
#include "graph/proxy/node.h"

void ScriptTitleHook::call(std::string title)
{
    proxy->getInspector()->setTitle(QString::fromStdString(title));
}
