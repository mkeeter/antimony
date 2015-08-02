#include "graph/hooks/export.h"
#include "graph/hooks/hooks.h"

#include "ui/canvas/graph_scene.h"
#include "export/export_mesh.h"
#include "export/export_heightmap.h"

#include <QString>

using namespace boost::python;

Shape ScriptExportHooks::get_shape(tuple args)
{
    extract<Shape> shape_(args[1]);
    if (!shape_.check())
        throw AppHooks::Exception(
                "First argument to export task must be a fab.types.Shape");
    return shape_();
}

Bounds ScriptExportHooks::get_bounds(dict kwargs)
{
    extract<Bounds> bounds_(kwargs["bounds"]);
    if (!bounds_.check())
        throw AppHooks::Exception(
                "bounds argument must be a fab.types.Bounds object");
    return bounds_();
}

Bounds ScriptExportHooks::pad_bounds(Bounds b)
{
    float dx = (b.xmax - b.xmin) / 20;
    float dy = (b.ymax - b.ymin) / 20;
    float dz = (b.zmax - b.zmin) / 20;
    return Bounds(b.xmin - dx, b.ymin - dy, b.zmin - dz,
                  b.xmax + dx, b.ymax + dy, b.zmax + dz);
}

bool ScriptExportHooks::get_pad(dict kwargs)
{
    if (!kwargs.has_key("pad"))
        return true;

    extract<bool> pad_(kwargs["pad"]);
    if (!pad_.check())
        throw AppHooks::Exception(
                "pad argument must be a boolean.");
    return pad_();
}

float ScriptExportHooks::get_resolution(dict kwargs)
{
    if (!kwargs.has_key("resolution"))
        return -1;

    extract<float> resolution_(kwargs["resolution"]);
    if (!resolution_.check())
        throw AppHooks::Exception(
                "resolution argument must be a float.");
    return resolution_();
}

QString ScriptExportHooks::get_filename(dict kwargs)
{
    if (!kwargs.has_key("filename"))
        return QString();

    extract<std::string> filename_(kwargs["filename"]);
    if (!filename_.check())
        throw AppHooks::Exception(
                "filename argument must be a string.");
    return QString::fromStdString(filename_());
}

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

    Bounds bounds = shape.bounds;
    if (kwargs.has_key("bounds"))
        bounds = get_bounds(kwargs);

    // Sanity-check bounds
    if (isinf(bounds.xmin) || isinf(bounds.xmax) ||
        isinf(bounds.ymin) || isinf(bounds.ymax) ||
        isinf(bounds.zmin) || isinf(bounds.zmax))
    {
        throw AppHooks::Exception(
                "Exporting mesh with invalid (infinite) bounds");
    }

    bool pad = get_pad(kwargs);

    if (pad)
        bounds = pad_bounds(bounds);

    QString filename = get_filename(kwargs);
    float resolution = get_resolution(kwargs);;

    bool detect_features = false;
    if (kwargs.has_key("detect_features"))
    {
        extract<bool> detect_features_(kwargs["detect_features"]);
        if (!detect_features_.check())
            throw AppHooks::Exception(
                    "detect_features argument must be a boolean.");
        detect_features = detect_features_();
    }

    self->scene->setExportWorker(self->node, new ExportMeshWorker(
                shape, bounds, filename, resolution, detect_features));
    return object();
}

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

    Bounds bounds = shape.bounds;
    if (kwargs.has_key("bounds"))
        bounds = get_bounds(kwargs);

    // Sanity-check bounds
    if (isinf(bounds.xmin) || isinf(bounds.xmax) ||
        isinf(bounds.ymin) || isinf(bounds.ymax))
    {
        throw AppHooks::Exception(
                "Exporting heightmap with invalid (infinite) bounds");
    }

    bool pad = get_pad(kwargs);

    if (pad)
        bounds = pad_bounds(bounds);

    QString filename = get_filename(kwargs);
    float resolution = get_resolution(kwargs);;

    float mm_per_unit = 25.4;
    if (kwargs.has_key("mm_per_unit"))
    {
        extract<bool> mm_per_unit_(kwargs["mm_per_unit"]);
        if (!mm_per_unit_.check())
            throw AppHooks::Exception(
                    "mm_per_unit argument must be a float.");
        mm_per_unit = mm_per_unit_();
    }

    self->scene->setExportWorker(self->node, new ExportHeightmapWorker(
                shape, bounds, filename, resolution, mm_per_unit));
    return object();
}
