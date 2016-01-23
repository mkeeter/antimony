#pragma once

#include "window/base.h"

class ViewportScene;

class ViewportWindow : public BaseWindow
{
public:
    ViewportWindow(ViewportScene* scene);
};
