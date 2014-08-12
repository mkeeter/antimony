#include "control/csg/union_control.h"
#include "ui/colors.h"

UnionControl::UnionControl(Canvas *canvas, Node *node)
    : TextLabelControl("A | B", Colors::green, canvas, node)
{
    // Nothing to do here
}
