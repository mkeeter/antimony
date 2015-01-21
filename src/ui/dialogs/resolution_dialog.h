#ifndef RESOLUTION_DIALOG_H
#define RESOLUTION_DIALOG_H

#include <QDialog>

namespace Ui {
class ResolutionDialog;
}

struct Shape;

#define RESOLUTION_DIALOG_2D 0
#define RESOLUTION_DIALOG_3D 1

class ResolutionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ResolutionDialog(Shape* s, bool dimensions, bool has_units,
                              long max_voxels=(1<<22), QWidget* parent=0);
    float getResolution() const;
    float getMMperUnit() const;
protected slots:
    void onValueChanged(int i);
protected:
    Shape* shape;
    Ui::ResolutionDialog* ui;
    bool z_bounded;
};

#endif
