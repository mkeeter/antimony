#include <cmath>

#include "ui_resolution_dialog.h"
#include "ui/resolution_dialog.h"
#include "fab/types/shape.h"

ResolutionDialog::ResolutionDialog(Shape* shape, QWidget* parent)
    : QDialog(parent), shape(shape), ui(new Ui::ResolutionDialog),
      z_bounded(!isinf(shape->bounds.zmax) && !isinf(shape->bounds.zmin))
{
    ui->setupUi(this);
    connect(ui->export_res, SIGNAL(valueChanged(int)),
            this, SLOT(onValueChanged(int)));
    ui->export_res->setValue(startResolution());
}

int ResolutionDialog::startResolution() const
{
    return std::min(std::min(
                64 / (shape->bounds.xmax - shape->bounds.xmin),
                64 / (shape->bounds.ymax - shape->bounds.ymin)),
                z_bounded ? 64 / (shape->bounds.zmax - shape->bounds.zmin)
                          : 1000);
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
