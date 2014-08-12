#include "ui_resolution_dialog.h"
#include "ui/resolution_dialog.h"
#include "cpp/shape.h"

ResolutionDialog::ResolutionDialog(Shape* shape, QWidget* parent)
    : QDialog(parent), shape(shape), ui(new Ui::ResolutionDialog)
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
                64 / (shape->bounds.ymax - shape->bounds.ymin));
}

void ResolutionDialog::onValueChanged(int i)
{
    ui->export_size->setText(QString("%1 x %2 x %3")
            .arg(int((shape->bounds.xmax - shape->bounds.xmin) * i))
            .arg(int((shape->bounds.ymax - shape->bounds.ymin) * i))
            .arg(int((shape->bounds.zmax - shape->bounds.zmin) * i)));
    // nothing to do here
}

float ResolutionDialog::getResolution() const
{
    return ui->export_res->value();
}
