#pragma once

#include "window/base.h"

struct Script;
class ScriptFrame;
struct ScriptState;

class ScriptWindow : public BaseWindow
{
Q_OBJECT

public:
    ScriptWindow(Script* script);

    void setText(QString text);
    void highlightError(int lineno);
    void setOutput(QString text);
    void setError(QString text);

public slots:
    void onStateChanged(const ScriptState& state);

protected:
    ScriptFrame* const frame;
};
