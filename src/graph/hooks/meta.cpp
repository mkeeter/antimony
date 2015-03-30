#include "graph/hooks/meta.h"
#include "graph/hooks/hooks.h"

#include "fab/types/bounds.h"
#include "fab/types/shape.h"

#include <QString>

using namespace boost::python;

object ScriptMetaHooks::export_stl(tuple args, dict kwargs)
{
    ScriptMetaHooks& self = extract<ScriptMetaHooks&>(args[0])();

    if (len(args) != 2)
        throw hooks::HookException(
                "export_stl must be called with shape as first argument.");

    PyObject_Print(extract<object>(args[1])().ptr(), stdout, 0);
    extract<Shape> shape_(args[1]);
    if (!shape_.check())
        throw hooks::HookException(
                "First argument to export_stl must be a fab.types.Shape");
    Shape shape = shape_();

    Bounds bounds = shape.bounds;
    if (kwargs.has_key("bounds"))
    {
        extract<Bounds> bounds_(kwargs["bounds"]);
        if (!bounds_.check())
            throw hooks::HookException(
                    "bounds argument must be a fab.types.Bounds object");
        bounds = bounds_();
    }

    // Sanity-check bounds
    if (isinf(bounds.xmin) || isinf(bounds.xmax) ||
        isinf(bounds.ymin) || isinf(bounds.ymax) ||
        isinf(bounds.zmin) || isinf(bounds.zmax))
    {
        throw hooks::HookException(
                "Exporting mesh with invalid (infinite) bounds");
    }

    QString filename;
    if (kwargs.has_key("filename"))
    {
        extract<std::string> filename_(kwargs["filename"]);
        if (!filename_.check())
            throw hooks::HookException(
                    "filename argument must be a string");
        filename = QString::fromStdString(filename_());
    }

    printf("export_stl called\n");
    return object();
}
