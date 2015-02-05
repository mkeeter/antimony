#include <cmath>

#include "ui_resolution_dialog.h"
#include "ui/dialogs/resolution_dialog.h"
#include "fab/types/shape.h"

ResolutionDialog::ResolutionDialog(Shape* shape, bool dimensions, bool has_units,
                                   long max_voxels, QWidget* parent)
    : QDialog(parent), shape(shape), ui(new Ui::ResolutionDialog),
      z_bounded(!isinf(shape->bounds.zmax) && !isinf(shape->bounds.zmin))
{
    ui->setupUi(this);

    if (!has_units)
    {
        ui->units->hide();
        ui->unit_label->hide();
        layout()->invalidate();
        adjustSize();
    }

    // This connection is awkward because of function overloading.
    connect(ui->export_res,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &ResolutionDialog::onValueChanged);

    if (dimensions == RESOLUTION_DIALOG_2D || !z_bounded)
    {
        float area = (shape->bounds.xmax - shape->bounds.xmin) *
                     (shape->bounds.ymax - shape->bounds.ymin);
        ui->export_res->setValue(pow(max_voxels / area, 1/2.) / 4.);
    }
    else
    {
        float volume = (shape->bounds.xmax - shape->bounds.xmin) *
                       (shape->bounds.ymax - shape->bounds.ymin) *
                       (shape->bounds.zmax - shape->bounds.zmin);
        ui->export_res->setValue(pow(max_voxels / volume, 1/3.) / 2.52);
    }
}

void ResolutionDialog::onValueChanged(int i)
{
    ui->export_size->setText(QString("%1 x %2 x %3")
            .arg(int((shape->bounds.xmax - shape->bounds.xmin) * i))
            .arg(int((shape->bounds.ymax - shape->bounds.ymin) * i))
            .arg(z_bounded
                    ? int((shape->bounds.zmax - shape->bounds.zmin) * i)
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
