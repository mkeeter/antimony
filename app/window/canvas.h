#pragma once

#include "window/base.h"

class CanvasView;

class CanvasWindow : public BaseWindow
{
public:
    CanvasWindow(CanvasView* view);
};
