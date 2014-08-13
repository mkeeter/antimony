#ifndef OFFSET_CONTROL_H
#define OFFSET_CONTROL_H

#include "control/text.h"

class OffsetControl : public TextLabelControl
{
public:
    explicit OffsetControl(Canvas* canvas, Node* node);
};

#endif

