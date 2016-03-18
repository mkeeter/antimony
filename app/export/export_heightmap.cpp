#include <Python.h>

#include <QFileDialog>
#include <QMessageBox>

#include "export/export_heightmap.h"

#include "dialog/resolution.h"

#include "fab/util/region.h"
#include "fab/tree/render.h"
#include "fab/formats/png.h"

////////////////////////////////////////////////////////////////////////////////

void ExportHeightmapWorker::run()
{
    // Sanity-check bounds
    if (std::isinf(bounds.xmin) || std::isinf(bounds.xmax) ||
        std::isinf(bounds.ymin) || std::isinf(bounds.ymax))
    {
        QMessageBox::critical(NULL, "Export error",
                "<b>Export error:</b><br>"
                "Target shape has invalid (infinite) bounds");
        return;
    }

    if (resolution == -1)
    {
        auto resolution_dialog = new ResolutionDialog(
                bounds, RESOLUTION_DIALOG_2D, HAS_UNITS);
        if (!resolution_dialog->exec())
            return;
        _resolution = resolution_dialog->getResolution();
        _mm_per_unit = resolution_dialog->getMMperUnit();
        delete resolution_dialog;
    }
    else
    {
        _resolution = resolution;
        _mm_per_unit = mm_per_unit;
    }

    if (_resolution == 0)
    {
        QMessageBox::critical(NULL, "Export error",
                "<b>Export error:</b><br>"
                "Resolution cannot be set to 0");
        return;
    }

    if (filename.isEmpty())
        _filename = QFileDialog::getSaveFileName(
                NULL, "Export .png", "", "*.png");
    else
        _filename = filename;
    if (_filename.isEmpty())
        return;

    if (checkWritable())
        runAsync();

    /*
     *  Check return code from async call
     */
    if (!success)
    {
        QMessageBox::critical(NULL, "Export error",
                            "<b>Writing to png file failed</b><br>"
                            "Check logs for error message with details.");
    }
}

////////////////////////////////////////////////////////////////////////////////

void ExportHeightmapWorker::async()
{
    Region r =  (Region){
        .imin=0, .jmin=0, .kmin=0,
        .ni=uint32_t((bounds.xmax - bounds.xmin) * _resolution),
        .nj=uint32_t((bounds.ymax - bounds.ymin) * _resolution),
        .nk=1
    };

    if (!std::isinf(bounds.zmin) && !std::isinf(bounds.zmax))
        r.nk = uint32_t((bounds.zmax - bounds.zmin) * _resolution);

    build_arrays(
            &r, bounds.xmin, bounds.ymin, bounds.zmin,
                bounds.xmax, bounds.ymax, bounds.zmax);

    uint16_t* d16(new uint16_t[r.ni * r.nj]);
    uint16_t** d16_rows(new uint16_t*[r.nj]);

    for (unsigned i=0; i < r.nj; ++i)
        d16_rows[i] = &d16[r.ni * i];

    memset(d16, 0, r.ni * r.nj * sizeof(uint16_t));
    render16(shape.tree.get(), r, d16_rows, &halt, NULL);

    // These bounds will be stored to give the .png real-world units.
    float bounds[6] = {
        r.X[0] * _mm_per_unit,
        r.Y[0] * _mm_per_unit,
        r.Z[0] * _mm_per_unit,
        r.X[r.ni] * _mm_per_unit,
        r.Y[r.nj] * _mm_per_unit,
        r.Z[r.nk] * _mm_per_unit};

    // Flip rows before saving image
    for (unsigned i=0; i < r.nj; ++i)
        d16_rows[r.nj - i - 1] = d16 + (r.ni * i);

    // If the operation has been cancelled, then mark it as a sucess;
    // otherwise, attempt to write the file and check the return code.
    if (halt)
        success = true;
    else
        success = save_png16L(_filename.toStdString().c_str(), r.ni, r.nj,
                              bounds, d16_rows);

    free_arrays(&r);
    delete [] d16;
    delete [] d16_rows;
}

