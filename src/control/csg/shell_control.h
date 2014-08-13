#ifndef SHELL_CONTROL_H
#define SHELL_CONTROL_H

#include "control/text.h"

class ShellControl : public TextLabelControl
{
public:
    explicit ShellControl(Canvas* canvas, Node* node);
};

#endif

