#ifndef RESOLUTION_DIALOG_H
#define RESOLUTION_DIALOG_H

#include <QDialog>

namespace Ui {
class ResolutionDialog;
}

class ResolutionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ResolutionDialog(QWidget* parent=0);
    float getResolution() const;
protected:
    Ui::ResolutionDialog* ui;
};

#endif
