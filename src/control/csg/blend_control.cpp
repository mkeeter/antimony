#include "control/csg/blend_control.h"
#include "ui/colors.h"

BlendControl::BlendControl(Canvas *canvas, Node *node)
    : TextLabelControl("blend", Colors::green, canvas, node)
{
    // Nothing to do here
}
