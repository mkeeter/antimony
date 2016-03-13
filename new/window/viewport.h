#pragma once

#include "window/base_viewport_window.h"

class ViewportView;

class ViewportWindow : public BaseViewportWindow
{
public:
    ViewportWindow(ViewportView* view);
};
