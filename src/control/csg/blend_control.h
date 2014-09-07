#ifndef BLEND_CONTROL_H
#define BLEND_CONTROL_H

#include "control/text.h"

class BlendControl : public TextLabelControl
{
public:
    explicit BlendControl(Canvas* canvas, Node* node);
};

#endif
