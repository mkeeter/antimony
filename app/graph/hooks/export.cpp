#include "graph/hooks/export.h"
#include "graph/hooks/hooks.h"

#include "graph/proxy/node.h"
#include "export/export_mesh.h"
#include "export/export_heightmap.h"

#include <QString>

using namespace boost::python;

////////////////////////////////////////////////////////////////////////////////

template <class T>
T ScriptExportHooks::get_object(std::string name, dict kwargs, T d)
{
    if (!kwargs.has_key(name))
        return d;

    extract<T> out(kwargs[name]);
    if (!out.check())
        throw AppHooks::Exception(
                "Keyword argument " + name + " is of the wrong type.");
    return out();
}

////////////////////////////////////////////////////////////////////////////////

Shape ScriptExportHooks::get_shape(tuple args)
{
    extract<Shape> shape_(args[1]);
    if (!shape_.check())
        throw AppHooks::Exception(
                "First argument to export task must be a fab.types.Shape");
    return shape_();
}

////////////////////////////////////////////////////////////////////////////////

Bounds ScriptExportHooks::pad_bounds(Bounds b)
{
    float dx = (b.xmax - b.xmin) / 20;
    float dy = (b.ymax - b.ymin) / 20;
    float dz = (b.zmax - b.zmin) / 20;
    return Bounds(b.xmin - dx, b.ymin - dy, b.zmin - dz,
                  b.xmax + dx, b.ymax + dy, b.zmax + dz);
}

////////////////////////////////////////////////////////////////////////////////

object ScriptExportHooks::stl(tuple args, dict kwargs)
{
    ScriptExportHooks* self = extract<ScriptExportHooks*>(args[0])();

    if (self->called)
        throw AppHooks::Exception(
                "Cannot define multiple export tasks in a single script.");
    self->called = true;

    if (len(args) != 2)
        throw AppHooks::Exception(
                "export.stl must be called with shape as first argument.");

    Shape shape = get_shape(args);
    Bounds bounds = get_object("bounds", kwargs, shape.bounds);

    if (get_object("pad", kwargs, true))
        bounds = pad_bounds(bounds);

    const QString filename = QString::fromStdString(
            get_object("filename", kwargs, ""));
    const float resolution = get_object("resolution", kwargs, -1);
    const bool detect_features = get_object("detect_features", kwargs, false);

    self->proxy->setExportWorker(new ExportMeshWorker(
                shape, bounds, filename, resolution, detect_features));
    return object();
}

////////////////////////////////////////////////////////////////////////////////

object ScriptExportHooks::heightmap(tuple args, dict kwargs)
{
    ScriptExportHooks* self = extract<ScriptExportHooks*>(args[0])();

    if (self->called)
        throw AppHooks::Exception(
                "Cannot define multiple export tasks in a single script.");
    self->called = true;

    if (len(args) != 2)
        throw AppHooks::Exception(
                "export.stl must be called with shape as first argument.");

    Shape shape = get_shape(args);
    Bounds bounds = get_object("bounds", kwargs, shape.bounds);

    if (get_object("pad", kwargs, true))
        bounds = pad_bounds(bounds);

    const QString filename = QString::fromStdString(
            get_object("filename", kwargs, ""));
    const float resolution = get_object("resolution", kwargs, -1);
    const float mm_per_unit = get_object("mm_per_unit", kwargs, 25.4);

    self->proxy->setExportWorker(new ExportHeightmapWorker(
                shape, bounds, filename, resolution, mm_per_unit));
    return object();
}
