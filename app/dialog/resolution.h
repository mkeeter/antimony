#pragma once

#include <QDialog>

#include "fab/types/bounds.h"

namespace Ui {
class ResolutionDialog;
}

#define RESOLUTION_DIALOG_2D 0
#define RESOLUTION_DIALOG_3D 1

#define HAS_UNITS 1
#define UNITLESS 0

/*
 *  General-purpose dialog for getting 2D and 3D resolution
 */
class ResolutionDialog : public QDialog
{
    Q_OBJECT
public:
    /*
     *  Construct a new resolution dialog.
     *
     *  b is the export bounds (used for printing pixel / voxel counts)
     *  dimensions is RESOLUTION_DIALOG_2D or RESOLUTION_DIALOG_3D
     *  has_units is HAS_UNITS or UNITLESS
     *  max_voxels is the maximum allowable voxel count
     */
    explicit ResolutionDialog(Bounds b, bool dimensions, bool has_units,
                              long max_voxels=(1<<22), QWidget* parent=0);

    /*
     *  Returns the current resolution (from the UI)
     */
    float getResolution() const;

    /*
     *  Returns the current mm/unit setting
     */
    float getMMperUnit() const;

    /*
     *  Returns the state of the "Detect Features (experimental)" checkbox
     */
    bool getDetectFeatures() const;

protected slots:
    /*
     *  When a value changes, update pixel / voxel count
     */
    void onValueChanged(int i);

protected:
    Bounds bounds;
    Ui::ResolutionDialog* ui;
    const bool z_bounded;
};
