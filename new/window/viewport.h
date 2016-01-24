#pragma once

#include "window/base.h"

class ViewportView;

class ViewportWindow : public BaseWindow
{
public:
    ViewportWindow(ViewportView* view);
};
