#include "graph/hooks/title.h"
#include "ui/canvas/inspector/inspector.h"

void ScriptTitleHook::call(std::string name)
{
    if (inspector)
        inspector->setTitle(QString::fromStdString(name));
}
