#include "graph/hooks/ui.h"

using namespace boost::python;

object ScriptUIHooks::point(tuple args, dict kwargs)
{
    ScriptUIHooks& self = extract<ScriptUIHooks&>(args[0])();
    // Nothing to do here
}
