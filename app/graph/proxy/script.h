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

    /*
     *  On script state changes, update the UI
     */
    void trigger(const ScriptState& state) override;

public slots:
    /*
     *  Creates a new script window
     */
    void newScriptWindow();

signals:
    /*
     *  Instructs windows to update themselves
     */
    void stateChanged(const ScriptState& state);

    /*
     *  Signal connected to windows that renames them
     */
    void subnameChanged(QString n);

protected:
    Script* const script;
    InspectorScriptButton* button;
};
