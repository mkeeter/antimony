#include <Python.h>

#include "canvas/inspector/buttons.h"

#include "graph/proxy/script.h"
#include "graph/proxy/node.h"

#include "graph/script.h"
#include "window/script.h"

ScriptProxy::ScriptProxy(Script* s, NodeProxy* parent)
    : QObject(parent), script(s),
      button(new InspectorScriptButton(
                  this, parent->getInspector()->getTitleRow()))
{
    s->installWatcher(this);
}

void ScriptProxy::trigger(const ScriptState& state)
{
    button->setScriptValid(state.error_lineno == -1);
}

void ScriptProxy::newScriptWindow()
{
    auto win = new ScriptWindow();

    // Automatically prune the window list when the window is closed
    connect(win, &QMainWindow::destroyed,
            [=]{ this->windows.removeAll(win); });
    windows.append(win);

    trigger(script->getState());
}
