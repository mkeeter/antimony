#pragma once

#include "graph/types/source.h"

class Input : public Source
{
public:
    PyObject* getValue() override;
    bool isActive() const;
};
