#include "ui_resolution_dialog.h"
#include "ui/resolution_dialog.h"

ResolutionDialog::ResolutionDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::ResolutionDialog)
{
    ui->setupUi(this);
}

float ResolutionDialog::getResolution() const
{
    return ui->export_res->value();
}
