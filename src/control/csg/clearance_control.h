#ifndef CLEARANCE_CONTROL_H
#define CLEARANCE_CONTROL_H

#include "control/text.h"

class ClearanceControl : public TextLabelControl
{
public:
    explicit ClearanceControl(Canvas* canvas, Node* node);
};

#endif

