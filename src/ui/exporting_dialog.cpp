#include "ui_exporting_dialog.h"
#include "ui/exporting_dialog.h"

ExportingDialog::ExportingDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::ExportingDialog)
{
    ui->setupUi(this);
}
