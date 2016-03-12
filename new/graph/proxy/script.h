#pragma once

#include <QObject>
#include <QHash>

#include "graph/watchers.h"

struct Script;
class NodeProxy;
class ScriptWindow;
class InspectorScriptButton;

class ScriptProxy : public QObject, public ScriptWatcher
{
Q_OBJECT

public:
    ScriptProxy(Script* s, NodeProxy* parent);

    void trigger(const ScriptState& state) override;

public slots:
    void newScriptWindow();

signals:
    void stateChanged(const ScriptState& state);

protected:
    Script* const script;
    InspectorScriptButton* button;
};
