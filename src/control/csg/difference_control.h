#ifndef DIFFERENCE_CONTROL_H
#define DIFFERENCE_CONTROL_H

#include "control/text.h"

class DifferenceControl : public TextLabelControl
{
public:
    explicit DifferenceControl(Canvas* canvas, Node* node);
};

#endif
