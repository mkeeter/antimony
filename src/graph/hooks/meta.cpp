#include "graph/hooks/meta.h"

using namespace boost::python;

object ScriptMetaHooks::export_stl(tuple args, dict kwargs)
{
    printf("export_stl called\n");
    return object();
}
