#include "control/csg/shell_control.h"
#include "ui/colors.h"

ShellControl::ShellControl(Canvas *canvas, Node *node)
    : TextLabelControl("Shell", Colors::green, canvas, node)
{
    // Nothing to do here
}
