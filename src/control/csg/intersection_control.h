#ifndef INTERSECTION_CONTROL_H
#define INTERSECTION_CONTROL_H

#include "control/text.h"

class IntersectionControl : public TextLabelControl
{
public:
    explicit IntersectionControl(Canvas* canvas, Node* node);
};

#endif
