#include <Python.h>

#include <QFileDialog>
#include <QMessageBox>

#include "export/export_mesh.h"

#include "dialog/resolution.h"

#include "fab/util/region.h"
#include "fab/tree/triangulate.h"
#include "fab/formats/stl.h"

////////////////////////////////////////////////////////////////////////////////

void ExportMeshWorker::run()
{
    // Sanity-check bounds
    if (std::isinf(bounds.xmin) || std::isinf(bounds.xmax) ||
        std::isinf(bounds.ymin) || std::isinf(bounds.ymax) ||
        std::isinf(bounds.zmin) || std::isinf(bounds.zmax))
    {
        QMessageBox::critical(NULL, "Export error",
                "<b>Export error:</b><br>"
                "Target shape has invalid (infinite) bounds");
        return;
    }

    // Get resolution, either hardcoded or from the user
    if (resolution == -1)
    {
        auto resolution_dialog = new ResolutionDialog(
                bounds, RESOLUTION_DIALOG_3D, UNITLESS);
        if (!resolution_dialog->exec())
            return;
        _resolution = resolution_dialog->getResolution();
        _detect_features = resolution_dialog->getDetectFeatures();
        delete resolution_dialog;
    }
    else
    {
        _resolution = resolution;
        _detect_features = detect_features;
    }

    if (_resolution == 0)
    {
        QMessageBox::critical(NULL, "Export error",
                "<b>Export error:</b><br>"
                "Resolution cannot be set to 0");
        return;
    }

    //  Get a target filename, either hardcoded or from the user
    if (filename.isEmpty())
        _filename = QFileDialog::getSaveFileName(
                NULL, "Export STL", "", "*.stl");
    else
        _filename = filename;
    if (_filename.isEmpty())
        return;

    if (checkWritable())
        runAsync();
}

////////////////////////////////////////////////////////////////////////////////

void ExportMeshWorker::async()
{
    float* verts;
    unsigned count;

    Region r = (Region){
        .imin=0, .jmin=0, .kmin=0,
        .ni=uint32_t((bounds.xmax - bounds.xmin) * _resolution),
        .nj=uint32_t((bounds.ymax - bounds.ymin) * _resolution),
        .nk=uint32_t((bounds.zmax - bounds.zmin) * _resolution),
    };
    r.voxels = r.ni * r.nj * r.nk;

    build_arrays(
            &r, bounds.xmin, bounds.ymin, bounds.zmin,
                bounds.xmax, bounds.ymax, bounds.zmax);

    triangulate(shape.tree.get(), r, _detect_features, &halt, &verts, &count);

    save_stl(verts, count, _filename.toStdString().c_str());
    free_arrays(&r);
    free(verts);
}
