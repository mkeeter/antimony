#include "control/csg/intersection_control.h"
#include "ui/colors.h"

IntersectionControl::IntersectionControl(Canvas *canvas, Node *node)
    : TextLabelControl("A & B", Colors::blue, canvas, node)
{
    // Nothing to do here
}
