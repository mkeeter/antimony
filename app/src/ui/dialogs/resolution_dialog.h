#ifndef RESOLUTION_DIALOG_H
#define RESOLUTION_DIALOG_H

#include <QDialog>

#include "fab/types/bounds.h"

namespace Ui {
class ResolutionDialog;
}

#define RESOLUTION_DIALOG_2D 0
#define RESOLUTION_DIALOG_3D 1

#define HAS_UNITS 1
#define UNITLESS 0

class ResolutionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ResolutionDialog(Bounds b, bool dimensions, bool has_units,
                              long max_voxels=(1<<22), QWidget* parent=0);
    float getResolution() const;
    float getMMperUnit() const;
    bool getDetectFeatures() const;
protected slots:
    void onValueChanged(int i);
protected:
    Bounds bounds;
    Ui::ResolutionDialog* ui;
    bool z_bounded;
};

#endif
