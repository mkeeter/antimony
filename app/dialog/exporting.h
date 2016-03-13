#pragma once

#include <QDialog>

namespace Ui {
class ExportingDialog;
}

/*
 *  Dialog used by the export system
 */
class ExportingDialog : public QDialog
{
public:
    explicit ExportingDialog(QWidget* parent=0);
protected:
    Ui::ExportingDialog* ui;
};
