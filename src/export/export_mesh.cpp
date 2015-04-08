#include <Python.h>

#include <QCoreApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QThread>
#include <QTime>
#include <QDebug>

#include "export/export_mesh.h"

#include "ui/dialogs/resolution_dialog.h"
#include "ui/dialogs/exporting_dialog.h"

#include "fab/util/region.h"
#include "fab/tree/triangulate.h"
#include "fab/formats/stl.h"

void ExportMeshWorker::run()
{
    float _resolution = resolution;
    QString _filename = filename;
    bool _detect_features = detect_features;

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

    if (filename.isEmpty())
        _filename = QFileDialog::getSaveFileName(
                NULL, "Export STL", "", "*.stl");

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
    auto task = new ExportMeshTask(
            shape, bounds, _resolution,
            _detect_features, _filename, &halt);
    task->moveToThread(thread);

    connect(thread, &QThread::started,
            task, &ExportMeshTask::render);
    connect(task, &ExportMeshTask::finished,
            thread, &QThread::quit);
    connect(thread, &QThread::finished,
            thread, &QThread::deleteLater);
    connect(thread, &QThread::finished,
            task, &ExportMeshTask::deleteLater);
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

void ExportMeshTask::render()
{
    float* verts;
    unsigned count;

    Region r = (Region){
        .imin=0, .jmin=0, .kmin=0,
        .ni=uint32_t((bounds.xmax - bounds.xmin) * resolution),
        .nj=uint32_t((bounds.ymax - bounds.ymin) * resolution),
        .nk=uint32_t((bounds.zmax - bounds.zmin) * resolution),
    };
    r.voxels = r.ni * r.nj * r.nk;

    build_arrays(
            &r, bounds.xmin, bounds.ymin, bounds.zmin,
                bounds.xmax, bounds.ymax, bounds.zmax);

    QTime time;
    time.start();
    triangulate(shape.tree.get(), r, detect_features, halt, &verts, &count);
    qDebug() << time.elapsed();

    save_stl(verts, count, filename.toStdString().c_str());
    free_arrays(&r);
    free(verts);

    qDebug() << "Done.";
    emit(finished());
}
