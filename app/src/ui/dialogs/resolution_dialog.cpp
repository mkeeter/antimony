#include <Python.h>
#include <cmath>

#include "ui_resolution_dialog.h"
#include "ui/dialogs/resolution_dialog.h"

ResolutionDialog::ResolutionDialog(Bounds bounds, bool dimensions, bool has_units,
                                   long max_voxels, QWidget* parent)
    : QDialog(parent), bounds(bounds), ui(new Ui::ResolutionDialog),
      z_bounded(!isinf(bounds.zmax) && !isinf(bounds.zmin))
{
    ui->setupUi(this);

    if (!has_units)
    {
        ui->units->hide();
        ui->unit_label->hide();
    }

    if (dimensions == RESOLUTION_DIALOG_2D)
        ui->detect_features->hide();

    // Re-do the layout, since things may have just been hidden
    layout()->invalidate();
    adjustSize();

    // This connection is awkward because of function overloading.
    connect(ui->export_res,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &ResolutionDialog::onValueChanged);

    if (dimensions == RESOLUTION_DIALOG_2D || !z_bounded)
    {
        float area = (bounds.xmax - bounds.xmin) *
                     (bounds.ymax - bounds.ymin);
        ui->export_res->setValue(pow(max_voxels / area, 1/2.) / 4.);
    }
    else
    {
        float volume = (bounds.xmax - bounds.xmin) *
                       (bounds.ymax - bounds.ymin) *
                       (bounds.zmax - bounds.zmin);
        ui->export_res->setValue(pow(max_voxels / volume, 1/3.) / 2.52);
    }
}

void ResolutionDialog::onValueChanged(int i)
{
    ui->export_size->setText(QString("%1 x %2 x %3")
            .arg(int((bounds.xmax - bounds.xmin) * i))
            .arg(int((bounds.ymax - bounds.ymin) * i))
            .arg(z_bounded
                    ? int((bounds.zmax - bounds.zmin) * i)
                    : 1));
}

float ResolutionDialog::getResolution() const
{
    return ui->export_res->value();
}


float ResolutionDialog::getMMperUnit() const
{
    QString u = ui->units->currentText();

    if (u == "mm")              return 1;
    else if (u == "cm")         return 10;
    else if (u == "inches")     return 25.4;

    return 1;
}

bool ResolutionDialog::getDetectFeatures() const
{
    return ui->detect_features->isChecked();
}
