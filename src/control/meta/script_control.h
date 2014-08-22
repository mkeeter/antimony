#ifndef SCRIPT_CONTROL_H
#define SCRIPT_CONTROL_H

#include "control/text.h"

class ScriptControl : public TextLabelControl
{
public:
    explicit ScriptControl(Canvas* canvas, Node* node);
protected:
    QString getLabel() const override;
};

#endif // SCRIPT_CONTROL_H
