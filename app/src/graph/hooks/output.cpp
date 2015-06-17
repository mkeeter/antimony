#include "graph/hooks/output.h"
#include "graph/hooks/hooks.h"

#include "graph/datum/datums/script_datum.h"

void ScriptOutputHook::call(std::string name, boost::python::object obj)
{
    if (datum)
        datum->makeOutput(QString::fromStdString(name), obj.ptr());
}
