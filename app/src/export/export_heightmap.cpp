#include <Python.h>

#include <QCoreApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QThread>
#include <QTime>
#include <QDebug>

#include "export/export_heightmap.h"

#include "ui/dialogs/resolution_dialog.h"
#include "ui/dialogs/exporting_dialog.h"

#include "fab/util/region.h"
#include "fab/tree/render.h"
#include "fab/formats/png.h"

////////////////////////////////////////////////////////////////////////////////

void ExportHeightmapWorker::run()
{
    float _resolution = resolution;
    float _mm_per_unit = mm_per_unit;
    QString _filename = filename;

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

    if (filename.isEmpty())
        _filename = QFileDialog::getSaveFileName(
                NULL, "Export .png", "", "*.png");

    if (_filename.isEmpty())
        return;

    if (!QFileInfo(QFileInfo(_filename).path()).isWritable())
    {
        QMessageBox::critical(NULL, "Export error",
                "<b>Export error:</b><br>"
                "Target file is not writable.");
        return;
    }

    auto exporting_dialog = new ExportingDialog();

    int halt = 0;
    auto thread = new QThread();
    auto task = new ExportHeightmapTask(
            shape, bounds, _resolution,
            _mm_per_unit, _filename, &halt);
    task->moveToThread(thread);

    connect(thread, &QThread::started,
            task, &ExportHeightmapTask::render);
    connect(task, &ExportHeightmapTask::finished,
            thread, &QThread::quit);
    connect(thread, &QThread::finished,
            thread, &QThread::deleteLater);
    connect(thread, &QThread::finished,
            task, &ExportHeightmapTask::deleteLater);
    connect(thread, &QThread::destroyed,
            exporting_dialog, &ExportingDialog::accept);

    thread->start();
    // If the dialog was cancelled, set the halt flag and wait for the
    // thread to finish (processing events all the while).
    if (exporting_dialog->exec() == QDialog::Rejected)
    {
        halt = 1;
        while (thread->isRunning())
            QCoreApplication::processEvents();
    }
    delete exporting_dialog;
}

////////////////////////////////////////////////////////////////////////////////

void ExportHeightmapTask::render()
{
    Region r =  (Region){
        .imin=0, .jmin=0, .kmin=0,
        .ni=uint32_t((bounds.xmax - bounds.xmin) * resolution),
        .nj=uint32_t((bounds.ymax - bounds.ymin) * resolution),
        .nk=1
    };

    if (!isinf(bounds.zmin) && !isinf(bounds.zmax))
        r.nk = uint32_t((bounds.zmax - bounds.zmin) * resolution);

    build_arrays(
            &r, bounds.xmin, bounds.ymin, bounds.zmin,
                bounds.xmax, bounds.ymax, bounds.zmax);


    uint16_t* d16(new uint16_t[r.ni * r.nj]);
    uint16_t** d16_rows(new uint16_t*[r.nj]);

    for (unsigned i=0; i < r.nj; ++i)
        d16_rows[i] = &d16[r.ni * i];

    memset(d16, 0, r.ni * r.nj * sizeof(uint16_t));
    render16(shape.tree.get(), r, d16_rows, halt, NULL);

    // These bounds will be stored to give the .png real-world units.
    float bounds[6] = {
        r.X[0] * mm_per_unit,
        r.Y[0] * mm_per_unit,
        r.Z[0] * mm_per_unit,
        r.X[r.ni] * mm_per_unit,
        r.Y[r.nj] * mm_per_unit,
        r.Z[r.nk] * mm_per_unit};

    // Flip rows before saving image
    for (unsigned i=0; i < r.nj; ++i)
        d16_rows[r.nj - i - 1] = d16 + (r.ni * i);

    if (!*halt)
        save_png16L(filename.toStdString().c_str(), r.ni, r.nj,
                    bounds, d16_rows);

    free_arrays(&r);
    delete [] d16;
    delete [] d16_rows;
    emit(finished());
}
