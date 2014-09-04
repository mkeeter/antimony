#include <cmath>

#include "ui_resolution_dialog.h"
#include "ui/resolution_dialog.h"
#include "fab/types/shape.h"

ResolutionDialog::ResolutionDialog(Shape* shape, bool is_2d, long max_voxels,
                                   QWidget* parent)
    : QDialog(parent), shape(shape), ui(new Ui::ResolutionDialog),
      z_bounded(!isinf(shape->bounds.zmax) && !isinf(shape->bounds.zmin))
{
    ui->setupUi(this);
    connect(ui->export_res, SIGNAL(valueChanged(int)),
            this, SLOT(onValueChanged(int)));

    if (is_2d || !z_bounded)
    {
        float area = (shape->bounds.xmax - shape->bounds.xmin) *
                     (shape->bounds.ymax - shape->bounds.ymin);
        ui->export_res->setMaximum(pow(max_voxels / area, 1/2.));
        ui->export_res->setValue(ui->export_res->maximum() / 4);
    }
    else
    {
        float volume = (shape->bounds.xmax - shape->bounds.xmin) *
                       (shape->bounds.ymax - shape->bounds.ymin) *
                       (shape->bounds.zmax - shape->bounds.zmin);
        ui->export_res->setMaximum(pow(max_voxels / volume, 1/3.));
        ui->export_res->setValue(ui->export_res->maximum() / 2.52);
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
