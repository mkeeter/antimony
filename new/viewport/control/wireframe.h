#pragma once

#include <Python.h>

#include "viewport/control/control.h"

class WireframeControl : public Control
{
public:
    WireframeControl(NodeProxy* node, PyObject* drag_func=NULL);
};
