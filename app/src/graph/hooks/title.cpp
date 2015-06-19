#include "graph/hooks/title.h"
#include "graph/hooks/hooks.h"

#include "graph/datum/datums/script_datum.h"

void ScriptTitleHook::call(std::string name)
{
    if (datum)
        datum->setTitle(QString::fromStdString(name));
}
