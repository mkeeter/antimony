#include <Python.h>

#include <QDir>
#include <QMessageBox>
#include <QCloseEvent>

#include "app/app.h"
#include "window/base.h"

// Initialize global window count (used to detect when last window closes)
int BaseWindow::window_count = 0;

BaseWindow::BaseWindow(QString type)
    : QMainWindow(), window_type(type), ui(new Ui::BaseWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    connectActions(App::instance());
    setShortcuts();

    window_count++;
}

BaseWindow::~BaseWindow()
{
    window_count--;
}

void BaseWindow::connectActions(App* app)
{
    // File menu
    connect(ui->actionSave, &QAction::triggered,
            app, &App::onSave);
    connect(ui->actionSaveAs, &QAction::triggered,
            app, &App::onSaveAs);
    connect(ui->actionNew, &QAction::triggered,
            app, &App::onNew);
    connect(ui->actionOpen, &QAction::triggered,
            app, &App::onOpen);
    connect(ui->actionQuit, &QAction::triggered,
            app, &App::onQuit);
    connect(ui->actionClose, &QAction::triggered,
            this, &BaseWindow::tryClose);

    // Add undo / redo to edit menu
    ui->menuEdit->addAction(App::instance()->getUndoAction());
    ui->menuEdit->addAction(App::instance()->getRedoAction());

    // View window
    connect(ui->actionNewCanvas, &QAction::triggered,
            app, &App::newCanvasWindow);
    connect(ui->actionNewViewport, &QAction::triggered,
            app, &App::newViewportWindow);
    connect(ui->actionNewQuad, &QAction::triggered,
            app, &App::newQuadWindow);

    // Help menu
    connect(ui->actionAbout, &QAction::triggered,
            app, &App::onAbout);
    connect(ui->actionCheckUpdate, &QAction::triggered,
            app, &App::onUpdateCheck);

    // Connect to the App-level signals that adjust window titles
    connect(app, &App::filenameChanged,
            this, &BaseWindow::setFilename);
    connect(app, &App::cleanChanged,
            this, &BaseWindow::setClean);

    // Ask the app to emit those signals to set our title
    App::instance()->onNewWindow();
}

////////////////////////////////////////////////////////////////////////////////

void BaseWindow::setFilename(QString f)
{
    filename = f;
    updateTitle();
}

void BaseWindow::setClean(bool c)
{
    clean = c;
    updateTitle();
}

void BaseWindow::setSub(QString s)
{
    sub = s;
    updateTitle();
}

void BaseWindow::updateTitle()
{
    auto title = window_type + " [";
    if (filename.isEmpty())
    {
        title += "untitled";
    }
    else
    {
        // Use a shorter version of the filename if we're in a subgraph
        title += sub.isEmpty()
            ? filename
            : filename.split(QDir::separator()).last().replace(".sb", "");
    }

    if (!sub.isEmpty())
    {
        title += "." + sub;
    }

    title += "]";
    if (!clean)
    {
        title += "*";
    }
    setWindowTitle(title);
}

////////////////////////////////////////////////////////////////////////////////

void BaseWindow::setShortcuts()
{
    ui->actionNew->setShortcuts(QKeySequence::New);
    ui->actionOpen->setShortcuts(QKeySequence::Open);
    ui->actionSave->setShortcuts(QKeySequence::Save);
    ui->actionSaveAs->setShortcuts(QKeySequence::SaveAs);
    ui->actionClose->setShortcuts(QKeySequence::Close);
    ui->actionQuit->setShortcuts(QKeySequence::Quit);
    ui->actionCut->setShortcuts(QKeySequence::Cut);
    ui->actionCopy->setShortcuts(QKeySequence::Copy);
    ui->actionPaste->setShortcuts(QKeySequence::Paste);
}

////////////////////////////////////////////////////////////////////////////////

bool BaseWindow::askClose()
{
    if(window_count <= 1 && !App::instance()->isUndoStackClean())
    {
        auto res = QMessageBox::question(
                this, "Antimony", "There are unsaved changes!\n"
                                  "Do you still want to close this window?\n",
                QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);

        return (res == QMessageBox::Yes);
    }
    return true;
}

void BaseWindow::closeEvent(QCloseEvent* event)
{
    if (!askClose())
    {
        event->ignore();
    }
}

void BaseWindow::tryClose()
{
    if (askClose())
    {
        deleteLater();
    }
}
