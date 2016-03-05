#include "app/app.h"
#include "window/base.h"

BaseWindow::BaseWindow(QString type)
    : QMainWindow(), window_type(type), ui(new Ui::BaseWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    connectActions(App::instance());
    setShortcuts();

    setWindowTitle(type);
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
            this, &QObject::deleteLater);

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
}

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
