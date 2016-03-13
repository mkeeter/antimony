#include <Python.h>

#include "canvas/inspector/buttons.h"

#include "graph/proxy/script.h"
#include "graph/proxy/node.h"

#include "graph/script.h"
#include "window/script_window.h"

ScriptProxy::ScriptProxy(Script* s, NodeProxy* parent)
    : QObject(parent), script(s),
      button(new InspectorScriptButton(
                  this, parent->getInspector()->getTitleRow()))
{
    s->installWatcher(this);
}

////////////////////////////////////////////////////////////////////////////////

void ScriptProxy::trigger(const ScriptState& state)
{
    button->setScriptValid(state.error_lineno == -1);
    emit(stateChanged(state));
}

void ScriptProxy::newScriptWindow()
{
    auto win = new ScriptWindow(script);

    connect(this, &QObject::destroyed, win, &QObject::deleteLater);
    connect(this, &ScriptProxy::stateChanged,
            win, &ScriptWindow::onStateChanged);
    connect(this, &ScriptProxy::subnameChanged,
            win, &BaseWindow::setSub);

    trigger(script->getState());
    win->show();
}
