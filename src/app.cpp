#include <Python.h>

#include <QMessageBox>
#include <QFileDialog>

#include "app.h"

#include "ui_main_window.h"
#include "ui/main_window.h"
#include "ui/resolution_dialog.h"
#include "ui/canvas.h"
#include "ui/colors.h"

#include "node/manager.h"

App::App(int argc, char* argv[]) :
    QApplication(argc, argv), window(new MainWindow)
{
    setShortcuts();
    setGlobalStyle();
    connectActions();
    window->show();
}

App::~App()
{
    NodeManager::manager()->clear();
    delete window;
}

App* App::instance()
{
    return dynamic_cast<App*>(QApplication::instance());
}

Canvas* App::getCanvas() const
{
    return window->canvas;
}

MainWindow* App::getWindow() const
{
    return window;
}

void App::onAbout()
{
    QMessageBox::about(NULL, "antimony",
            "<b>antimony</b><br><br>"
            "A non-traditional CAD tool.<br>"
            "<a href=\"https://github.com/mkeeter/antimony\">https://github.com/mkeeter/antimony</a><br><br>"
            "© 2013 Matthew Keeter<br>"
            "<a href=\"mailto:matt.j.keeter@gmail.com\">matt.j.keeter@gmail.com</a><br>"
            "_________________________________________________<br><br>"
            "Includes code from <a href=\"https://github.com/mkeeter/kokopelli\">kokopelli</a>, which is <br>"
            "© 2012-2013 Massachusetts Institute of Technology<br>"
            "© 2013 Matthew Keeter<br><br>"
            "Inspired by the <a href=\"http://kokompe.cba.mit.edu\">fab modules</a>"
            );
}

void App::onNew()
{
    NodeManager::manager()->clear();
}

void App::onSave()
{
    if (filename.isEmpty())
    {
        return onSaveAs();
    }

    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    file.write(NodeManager::manager()->getSerializedScene());
}

void App::onSaveAs()
{
    QString f = QFileDialog::getSaveFileName(window, "Save as", "", "*.sb");
    if (!f.isEmpty())
    {
        filename = f;
        return onSave();
    }
}

void App::onOpen()
{
    QString f = QFileDialog::getOpenFileName(window, "Open", "", "*.sb");
    if (!f.isEmpty())
    {
        filename = f;
        NodeManager::manager()->clear();
        QFile file(f);
        file.open(QIODevice::ReadOnly);
        NodeManager::manager()->deserializeScene(file.readAll());
        NodeManager::manager()->makeControls(window->canvas);
        NodeManager::manager()->makeConnections(window->canvas);
    }
}

void App::onExportSTL()
{
    ResolutionDialog* d = new ResolutionDialog();
    if (!d->exec())
    {
        return;
    }
    QString f = QFileDialog::getSaveFileName(window, "Export", "", "*.stl");
    qDebug() << d->getResolution();
}

void App::setShortcuts()
{
    window->ui->actionNew->setShortcuts(QKeySequence::New);
    window->ui->actionOpen->setShortcuts(QKeySequence::Open);
    window->ui->actionSave->setShortcuts(QKeySequence::Save);
    window->ui->actionSaveAs->setShortcuts(QKeySequence::SaveAs);
    window->ui->actionQuit->setShortcuts(QKeySequence::Quit);
}

void App::connectActions()
{
    connect(window->ui->actionQuit, SIGNAL(triggered()),
            this, SLOT(quit()));
    connect(window->ui->actionAbout, SIGNAL(triggered()),
            this, SLOT(onAbout()));
    connect(window->ui->actionSave, SIGNAL(triggered()),
            this, SLOT(onSave()));
    connect(window->ui->actionSaveAs, SIGNAL(triggered()),
            this, SLOT(onSaveAs()));
    connect(window->ui->actionNew, SIGNAL(triggered()),
            this, SLOT(onNew()));
    connect(window->ui->actionOpen, SIGNAL(triggered()),
            this, SLOT(onOpen()));
    connect(window->ui->actionExportMesh, SIGNAL(triggered()),
            this, SLOT(onExportSTL()));
}

void App::setGlobalStyle()
{
    setStyleSheet(QString(
            "QToolTip {"
            "   color: %1;"
            "background-color: %2;"
            "border: 1px solid %1;"
            "}").arg(Colors::base03.name())
                .arg(Colors::base04.name()));
}
