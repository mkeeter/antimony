#include <Python.h>

#include "window/script_window.h"
#include "script/frame.h"
#include "script/editor.h"

#include "graph/watchers.h"

ScriptWindow::ScriptWindow(Script* script)
    : BaseWindow("Script"), frame(new ScriptFrame(script, this))
{
    setCentralWidget(frame);

    ui->menuView->deleteLater();
    ui->menuAdd->deleteLater();

    resize(600, 800);
}

////////////////////////////////////////////////////////////////////////////////

void ScriptWindow::onStateChanged(const ScriptState& state)
{
    frame->getEditor()->setText(QString::fromStdString(state.script));
    frame->getEditor()->highlightError(state.error_lineno);
    frame->setOutput(QString::fromStdString(state.output));
    frame->setError(QString::fromStdString(state.error));
}
