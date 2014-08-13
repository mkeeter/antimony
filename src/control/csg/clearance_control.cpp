#include "control/csg/clearance_control.h"
#include "ui/colors.h"

ClearanceControl::ClearanceControl(Canvas *canvas, Node *node)
    : TextLabelControl("Clearance", Colors::green, canvas, node)
{
    // Nothing to do here
}

