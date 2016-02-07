#include <Python.h>

#include <QCoreApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QThread>
#include <QTime>
#include <QDebug>
#include <QDir>
#include <QTemporaryDir>

#include <stdlib.h>
#include <string.h>
#include <iomanip>
#include <fstream>

#include "export/export_voxels.h"

#include "ui/dialogs/resolution_dialog.h"
#include "ui/dialogs/exporting_dialog.h"

#include "fab/util/region.h"
#include "fab/tree/eval.h"
#include "fab/formats/lodepng.h"

#include "quazip/JlCompress.h"

void ExportVoxelsWorker::run()
{
    float _resolution = resolution;
    QString _filename = filename;

    if (resolution == -1)
    {
        auto resolution_dialog = new ResolutionDialog(
                bounds, RESOLUTION_DIALOG_3D, UNITLESS, NO_DETECT_FEATURES);
        if (!resolution_dialog->exec())
            return;
        _resolution = resolution_dialog->getResolution();
        delete resolution_dialog;
    }

    if (filename.isEmpty())
        _filename = QFileDialog::getSaveFileName(
                NULL, "Export Voxels", "", "*.svx");

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
    const float THRESH = -0.1;

    auto ni=uint32_t((bounds.xmax - bounds.xmin) * resolution);
    auto nj=uint32_t((bounds.ymax - bounds.ymin) * resolution);
    auto nk=uint32_t((bounds.zmax - bounds.zmin) * resolution);

    QTemporaryDir outputDir("svx-output");

    auto densityPath = outputDir.path() + "/density";
    QDir().mkdir(densityPath);

    std::ostringstream max_number_width_oss;
    max_number_width_oss << (nj - 1);
    size_t max_number_width = max_number_width_oss.str().length() + 1;

    std::vector<unsigned char> slice_data;
    slice_data.resize(ni * nk);

        for (unsigned j = 0; j < nj; ++j) {
            auto Y = bounds.ymin*(nj - j)/(float)nj + bounds.ymax*j/(float)nj;

            for (unsigned i = 0; i < ni; ++i) {
                auto X = bounds.xmin*(ni - i)/(float)ni + bounds.xmax*i/(float)ni;

                for(unsigned k = 0; k < nk; ++k) {
                    auto Z = bounds.zmin*(nk - k)/(float)nk + bounds.zmax*k/(float)nk;

                    float result = eval_f(shape.tree.get(), X, Y, Z);
                
                    unsigned char voxel_value = 0;
                    if(result > 0) {
                        voxel_value = 0;
                    }
                    else if(result < THRESH) {
                        voxel_value = 255;
                    }
                    else {
                        voxel_value = static_cast<unsigned char>( (result / THRESH) * 255);
                    }

                    slice_data[ni*k+i] = voxel_value;
                }
        }

        std::stringstream ss;
        ss <<  densityPath.toStdString();
        ss << "/slice";
        ss << std::setfill(' ') << std::setw(max_number_width);
        ss << j;
        ss << ".png";

        lodepng::State state;
		state.info_raw.colortype = LCT_GREY;
		state.info_raw.bitdepth = 8;
		state.info_png.color.colortype = LCT_GREY;
		state.info_png.color.bitdepth = 8;
		state.encoder.auto_convert = 0;
		std::vector<unsigned char> buffer;
		unsigned error = lodepng::encode(buffer, slice_data, ni, nk, state);
        lodepng::save_file(buffer, ss.str().c_str());
    }

    std::ofstream manifest_file(outputDir.path().toStdString() + "/manifest.xml");
    manifest_file << "<?xml version=\"1.0\"?>" << std::endl;
	manifest_file << "<grid gridSizeX = \"" << ni << "\" gridSizeY = \"" << nj << "\" gridSizeZ = \"" << nk << "\" voxelSize = \"1.0E-4\" subvoxelBits = \"8\">" << std::endl;
	manifest_file << " <channels>" << std::endl;
	manifest_file << "  <channel type = \"DENSITY\" slices = \"density/slice%" << max_number_width << "d.png\"/>" << std::endl;
	manifest_file << " </channels>" << std::endl;
	manifest_file << "</grid>" << std::endl;
    manifest_file.close();

    JlCompress::compressDir(filename, outputDir.path());

    emit(finished());
}
