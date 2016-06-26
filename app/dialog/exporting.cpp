// cmake doesn't find ui_exporting_dialog without this dummy line
#include "ui_exporting_dialog.h"
#include "dialog/exporting.h"

ExportingDialog::ExportingDialog(QWidget* parent)
    : QDialog(parent, Qt::CustomizeWindowHint|Qt::WindowTitleHint),
      ui(new Ui::ExportingDialog)
{
    ui->setupUi(this);
}
