#include <Python.h>

#include <QCoreApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QThread>
#include <QTime>
#include <QDebug>

#include "export/export_voxels.h"

#include "ui/dialogs/resolution_dialog.h"
#include "ui/dialogs/exporting_dialog.h"

#include "fab/util/region.h"
#include "fab/tree/eval.h"

void ExportVoxelsWorker::run()
{
    float _resolution = resolution;
    QString _filename = filename;

    if (resolution == -1)
    {
        auto resolution_dialog = new ResolutionDialog(
                bounds, RESOLUTION_DIALOG_3D, UNITLESS);
        if (!resolution_dialog->exec())
            return;
        _resolution = resolution_dialog->getResolution();
        delete resolution_dialog;
    }

    if (filename.isEmpty())
        _filename = QFileDialog::getSaveFileName(
                NULL, "Export Voxels", "", "*.voxels");

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
    auto task = new ExportVoxelsTask(
            shape, bounds, _resolution,
            _filename, &halt);
    task->moveToThread(thread);

    connect(thread, &QThread::started,
            task, &ExportVoxelsTask::render);
    connect(task, &ExportVoxelsTask::finished,
            thread, &QThread::quit);
    connect(thread, &QThread::finished,
            thread, &QThread::deleteLater);
    connect(thread, &QThread::finished,
            task, &ExportVoxelsTask::deleteLater);
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

void ExportVoxelsTask::render()
{
    auto ni=uint32_t((bounds.xmax - bounds.xmin) * resolution);
    auto nj=uint32_t((bounds.ymax - bounds.ymin) * resolution);
    auto nk=uint32_t((bounds.zmax - bounds.zmin) * resolution);

    for (unsigned k = 0; k < nk; ++k) {
        auto Z = bounds.zmin*(nk - k)/(float)nk + bounds.zmax*k/(float)nk;
        for (unsigned j = 0; j < nj; ++j) {
            auto Y = bounds.ymin*(nj - j)/(float)nj + bounds.ymax*j/(float)nj;
            for (unsigned i = 0; i < ni; ++i) {
                if(*halt) return;

                auto X  = bounds.xmin*(ni - i)/(float)ni + bounds.xmax*i/(float)ni;
                float result = eval_f(shape.tree.get(), X, Y, Z);
                printf("@ %f, %f, %f => %f\n", X, Y, Z, result);
            }
        }
    }

    emit(finished());
}
