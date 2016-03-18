#pragma once

#include <QList>
#include "window/base.h"

class ViewportView;

/*
 *  Parent class used for both ViewportWindow and QuadWindow
 */
class BaseViewportWindow : public BaseWindow
{
public:
    BaseViewportWindow(QList<ViewportView*> views);

    /*
     *  Checks whether the Shaded box is selected
     *  (if not, the Heightmap box should be selected)
     */
    bool isShaded() const;
};
