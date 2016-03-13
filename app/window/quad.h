#pragma once

#include "window/base_viewport_window.h"

class ViewportView;

class QuadWindow : public BaseViewportWindow
{
public:
    QuadWindow(ViewportView* front, ViewportView* top,
               ViewportView* side,  ViewportView* ortho);
};
