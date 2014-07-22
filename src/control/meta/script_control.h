#ifndef SCRIPT_CONTROL_H
#define SCRIPT_CONTROL_H

#include "control/text.h"

class ScriptControl : public TextLabelControl
{
    Q_OBJECT
public:
    explicit ScriptControl(Canvas* canvas, Node* node);
};

#endif // SCRIPT_CONTROL_H
