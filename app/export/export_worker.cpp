#include <Python.h>

#include <QCoreApplication>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QDialog>

#include "export/export_worker.h"
#include "dialog/exporting.h"

void ExportWorker::runAsync()
{
    auto exporting_dialog = new ExportingDialog();

    auto future = QtConcurrent::run(this, &ExportWorker::async);
    QFutureWatcher<void> watcher;
    watcher.setFuture(future);

    connect(&watcher, &decltype(watcher)::finished,
            exporting_dialog, &ExportingDialog::accept);

    // Run until the dialog closes, either because it was accepeted
    // (which indicates that the future has finished) or cancelled
    // (which indicates that someone his escape).
    //
    // If the dialog was cancelled, set the halt flag and wait for the
    // thread to finish (processing events all the while).
    if (exporting_dialog->exec() == QDialog::Rejected)
    {
        halt = 1;
        while (future.isRunning())
            QCoreApplication::processEvents();
    }

    // Reset halt so that we don't halt immediately on future runs
    halt = 0;
    delete exporting_dialog;
}

bool ExportWorker::checkWritable() const
{
    if (!QFileInfo(QFileInfo(_filename).path()).isWritable())
    {
        QMessageBox::critical(NULL, "Export error",
                "<b>Export error:</b><br>"
                "Target file is not writable.");
        return false;
    }
    return true;
}
