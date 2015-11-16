#include "window/script.h"
#include "script/frame.h"
#include "script/editor.h"

ScriptWindow::ScriptWindow(Script* script)
    : BaseWindow("Script"), frame(new ScriptFrame(script, this))
{
    setCentralWidget(frame);

    ui->menuView->deleteLater();
    ui->menuAdd->deleteLater();

    resize(600, 800);
}

////////////////////////////////////////////////////////////////////////////////

void ScriptWindow::setText(QString text)
{
    frame->getEditor()->setText(text);
}

void ScriptWindow::highlightError(int lineno)
{
    frame->getEditor()->highlightError(lineno);
}

void ScriptWindow::setOutput(QString text)
{
    frame->setOutput(text);
}

void ScriptWindow::setError(QString text)
{
    frame->setError(text);
}

