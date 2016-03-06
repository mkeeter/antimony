#pragma once

#include "window/base.h"

class ViewportView;

class ViewportWindow : public BaseWindow
{
public:
    ViewportWindow(ViewportView* view);

    /*
     *  Checks whether the Shaded box is selected
     *  (if not, the Heightmap box should be selected)
     */
    bool isShaded() const;
};
