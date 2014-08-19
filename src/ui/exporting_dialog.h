#ifndef EXPORTING_DIALOG_H
#define EXPORTING_DIALOG_H

#include <QDialog>

namespace Ui {
class ExportingDialog;
}

class ExportingDialog : public QDialog
{
public:
    explicit ExportingDialog(QWidget* parent=0);
protected:
    Ui::ExportingDialog* ui;
};

#endif
