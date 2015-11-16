#pragma once

#include "window/base.h"

struct Script;
class ScriptFrame;

class ScriptWindow : public BaseWindow
{
public:
    ScriptWindow(Script* script);

    void setText(QString text);
    void highlightError(int lineno);
    void setOutput(QString text);
    void setError(QString text);

protected:
    ScriptFrame* const frame;
};
