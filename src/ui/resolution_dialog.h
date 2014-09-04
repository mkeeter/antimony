#ifndef RESOLUTION_DIALOG_H
#define RESOLUTION_DIALOG_H

#include <QDialog>

namespace Ui {
class ResolutionDialog;
}

struct Shape;

class ResolutionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ResolutionDialog(Shape* s, long max_voxels=(1<<22),
                              QWidget* parent=0);
    float getResolution() const;
protected slots:
    void onValueChanged(int i);
protected:
    Shape* shape;
    Ui::ResolutionDialog* ui;
    bool z_bounded;
};

#endif
