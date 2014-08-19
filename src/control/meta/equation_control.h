#ifndef EQUATION_CONTROL_H
#define EQUATION_CONTROL_H

#include "control/text.h"

class EquationControl : public TextLabelControl
{
public:
    explicit EquationControl(Canvas* canvas, Node* node);
protected:
    QString getLabel() const override;
};

#endif // EQUATION_CONTROL_H
